"""
RANOnline EP7 AI System - Ollama LLM統一管理器 (Python版本)

功能特性:
- 支援多種LLM模型自動切換 (llama3, phi3, mistral, gemma, qwen2, deepseek-coder)
- 智能請求分流和負載均衡
- 異步流式API調用
- 自動重試機制和錯誤恢復
- 詳細異常日誌記錄
- 場景化模型選擇策略
- 參數化prompt模板管理
- 批次請求處理

作者: Jy技術團隊
日期: 2025-06-14
版本: 2.0.0
"""

import asyncio
import aiohttp
import json
import time
import logging
import threading
import queue
from typing import Dict, List, Optional, Any, Callable, Union
from dataclasses import dataclass, field
from enum import Enum
from datetime import datetime, timedelta
import uuid
import os
from pathlib import Path


class ScenarioType(Enum):
    """請求場景類型"""
    GENERAL_CHAT = "general_chat"
    CODE_GENERATION = "code_generation"
    TECHNICAL_SUPPORT = "technical_support"
    GAME_NARRATIVE = "game_narrative"
    DATA_ANALYSIS = "data_analysis"
    TRANSLATION = "translation"
    SUMMARIZATION = "summarization"
    QUESTION_ANSWERING = "question_answering"
    CREATIVE_WRITING = "creative_writing"
    DEBUGGING = "debugging"


@dataclass
class ModelInfo:
    """LLM模型信息結構"""
    name: str
    family: str = ""
    parameter_size: str = ""
    quantization_level: str = ""
    size: int = 0
    modified_at: Optional[datetime] = None
    digest: str = ""
    is_available: bool = False
    avg_response_time: float = 0.0
    success_count: int = 0
    error_count: int = 0
    
    @property
    def success_rate(self) -> float:
        """計算成功率"""
        total = self.success_count + self.error_count
        return self.success_count / total if total > 0 else 0.0


@dataclass
class LLMRequestConfig:
    """LLM請求配置"""
    prompt: str
    model: str = ""
    scenario: ScenarioType = ScenarioType.GENERAL_CHAT
    system_prompt: str = ""
    options: Dict[str, Any] = field(default_factory=dict)
    stream: bool = True
    max_retries: int = 3
    timeout_ms: int = 30000
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class LLMResponse:
    """LLM響應結果"""
    success: bool = False
    content: str = ""
    model: str = ""
    error_message: str = ""
    raw_response: Dict[str, Any] = field(default_factory=dict)
    response_time_ms: int = 0
    total_tokens: int = 0
    timestamp: Optional[datetime] = None
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class BatchRequestItem:
    """批次請求項目"""
    id: str
    config: LLMRequestConfig
    submit_time: datetime
    priority: int = 0


class OllamaLLMManager:
    """
    Ollama LLM統一管理器主類
    
    功能特性:
    - 多模型自動切換和負載均衡
    - 異步流式API調用
    - 自動重試和錯誤恢復
    - 詳細日誌記錄
    - 場景化模型選擇
    """
    
    def __init__(self, ollama_url: str = "http://localhost:11434"):
        """
        初始化LLM管理器
        
        Args:
            ollama_url: Ollama服務器URL
        """
        self.ollama_url = ollama_url.rstrip('/')
        self.available_models: Dict[str, ModelInfo] = {}
        self.model_selection_strategy = "balanced"
        self.global_options: Dict[str, Any] = {}
        self.auto_retry_enabled = True
        self.max_concurrent_requests = 3
        self.current_concurrent_requests = 0
        
        # 請求隊列和管理
        self.request_queue = queue.PriorityQueue()
        self.active_requests: Dict[str, Any] = {}
        self.pending_requests: Dict[str, LLMRequestConfig] = {}
        self.retry_counters: Dict[str, int] = {}
        
        # 統計信息
        self.total_requests = 0
        self.successful_requests = 0
        self.failed_requests = 0
        
        # 狀態管理
        self.is_initialized = False
        self.is_service_available = False
        
        # 線程控制
        self._queue_thread: Optional[threading.Thread] = None
        self._stop_event = threading.Event()
        
        # 日誌設置
        self._setup_logging()
        
        # 回調函數
        self.on_request_completed: Optional[Callable] = None
        self.on_stream_data: Optional[Callable] = None
        self.on_request_error: Optional[Callable] = None
        self.on_model_switched: Optional[Callable] = None
        
        self.logger.info("Ollama LLM Manager created")
    
    def _setup_logging(self):
        """設置日誌記錄"""
        # 創建日誌目錄
        log_dir = Path.home() / ".ranonline" / "logs"
        log_dir.mkdir(parents=True, exist_ok=True)
        
        # 設置日誌器
        self.logger = logging.getLogger("ollama_llm_manager")
        self.logger.setLevel(logging.INFO)
        
        # 文件處理器
        log_file = log_dir / f"llm_manager_{datetime.now().strftime('%Y-%m-%d')}.log"
        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(logging.INFO)
        
        # 控制台處理器
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.WARNING)
        
        # 格式化器
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        file_handler.setFormatter(formatter)
        console_handler.setFormatter(formatter)
        
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
    
    async def initialize(self) -> bool:
        """
        初始化LLM管理器
        
        Returns:
            是否初始化成功
        """
        self.logger.info(f"Initializing with Ollama URL: {self.ollama_url}")
        
        try:
            # 測試服務連接
            async with aiohttp.ClientSession() as session:
                async with session.get(
                    f"{self.ollama_url}/api/version",
                    timeout=aiohttp.ClientTimeout(total=5)
                ) as response:
                    if response.status == 200:
                        version_info = await response.json()
                        self.is_service_available = True
                        self.logger.info(f"Ollama service available, version: {version_info.get('version', 'unknown')}")
                    else:
                        self.logger.error(f"Ollama service returned status: {response.status}")
                        return False
        except Exception as e:
            self.logger.error(f"Failed to connect to Ollama service: {e}")
            return False
        
        # 刷新模型列表
        await self.refresh_model_list()
        
        # 載入配置
        self.load_configuration()
        
        # 啟動隊列處理線程
        self._start_queue_processor()
        
        self.is_initialized = True
        self.logger.info("Ollama LLM Manager initialized successfully")
        return True
    
    def _start_queue_processor(self):
        """啟動請求隊列處理線程"""
        self._queue_thread = threading.Thread(target=self._process_queue_loop, daemon=True)
        self._queue_thread.start()
        self.logger.info("Queue processor thread started")
    
    def _process_queue_loop(self):
        """請求隊列處理循環"""
        while not self._stop_event.is_set():
            try:
                if self.current_concurrent_requests >= self.max_concurrent_requests:
                    time.sleep(0.1)
                    continue
                
                try:
                    # 從隊列獲取請求（0.1秒超時）
                    priority, request_id, config = self.request_queue.get(timeout=0.1)
                    
                    # 在新線程中處理請求
                    threading.Thread(
                        target=self._process_request_sync,
                        args=(request_id, config),
                        daemon=True
                    ).start()
                    
                except queue.Empty:
                    continue
                    
            except Exception as e:
                self.logger.error(f"Error in queue processing loop: {e}")
                time.sleep(1)
    
    def _process_request_sync(self, request_id: str, config: LLMRequestConfig):
        """同步處理單個請求"""
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            loop.run_until_complete(self._process_request_async(request_id, config))
        finally:
            loop.close()
    
    async def _process_request_async(self, request_id: str, config: LLMRequestConfig):
        """異步處理單個請求"""
        self.current_concurrent_requests += 1
        start_time = time.time()
        
        try:
            # 選擇模型
            model_name = config.model or self.select_best_model(config.scenario)
            if not model_name:
                raise Exception("No suitable model available")
            
            # 檢查模型健康狀態
            if not self.is_model_healthy(model_name):
                backup_model = self.select_best_model(config.scenario)
                if backup_model and backup_model != model_name:
                    self.logger.info(f"Switching from unhealthy model {model_name} to {backup_model}")
                    model_name = backup_model
                    if self.on_model_switched:
                        self.on_model_switched(config.model, model_name, "Model health issue")
                else:
                    raise Exception("No healthy model available")
            
            # 構建請求
            payload = self._build_request_payload(config)
            payload["model"] = model_name
            
            # 發送請求
            async with aiohttp.ClientSession() as session:
                timeout = aiohttp.ClientTimeout(total=config.timeout_ms / 1000)
                
                async with session.post(
                    f"{self.ollama_url}/api/generate",
                    json=payload,
                    timeout=timeout
                ) as response:
                    
                    if response.status != 200:
                        raise Exception(f"HTTP {response.status}: {await response.text()}")
                    
                    if config.stream:
                        await self._handle_stream_response(request_id, response, model_name, start_time)
                    else:
                        await self._handle_regular_response(request_id, response, model_name, start_time)
        
        except Exception as e:
            response_time = int((time.time() - start_time) * 1000)
            self._handle_request_error(request_id, str(e), config, response_time)
        
        finally:
            self.current_concurrent_requests -= 1
            self.active_requests.pop(request_id, None)
            self.pending_requests.pop(request_id, None)
    
    async def _handle_stream_response(self, request_id: str, response: aiohttp.ClientResponse, 
                                    model_name: str, start_time: float):
        """處理流式響應"""
        content_chunks = []
        
        async for line in response.content:
            line_text = line.decode('utf-8').strip()
            if not line_text:
                continue
            
            try:
                data = json.loads(line_text)
                chunk = data.get('response', '')
                done = data.get('done', False)
                
                if chunk:
                    content_chunks.append(chunk)
                    if self.on_stream_data:
                        self.on_stream_data(request_id, chunk, done)
                
                if done:
                    # 流式響應完成
                    response_time = int((time.time() - start_time) * 1000)
                    full_content = ''.join(content_chunks)
                    
                    llm_response = LLMResponse(
                        success=True,
                        content=full_content,
                        model=model_name,
                        response_time_ms=response_time,
                        timestamp=datetime.now(),
                        raw_response=data
                    )
                    
                    self._update_model_statistics(model_name, True, response_time)
                    self._log_request(request_id, llm_response)
                    
                    if self.on_request_completed:
                        self.on_request_completed(request_id, llm_response)
                    
                    self.successful_requests += 1
                    break
                    
            except json.JSONDecodeError:
                continue
    
    async def _handle_regular_response(self, request_id: str, response: aiohttp.ClientResponse,
                                     model_name: str, start_time: float):
        """處理常規響應"""
        response_time = int((time.time() - start_time) * 1000)
        data = await response.json()
        
        content = data.get('response', '')
        if content:
            llm_response = LLMResponse(
                success=True,
                content=content,
                model=model_name,
                response_time_ms=response_time,
                timestamp=datetime.now(),
                raw_response=data
            )
            
            self._update_model_statistics(model_name, True, response_time)
            self._log_request(request_id, llm_response)
            
            if self.on_request_completed:
                self.on_request_completed(request_id, llm_response)
            
            self.successful_requests += 1
        else:
            error_msg = data.get('error', 'Empty response')
            self._handle_request_error(request_id, error_msg, None, response_time)
    
    def _handle_request_error(self, request_id: str, error_msg: str, 
                            config: Optional[LLMRequestConfig], response_time: int):
        """處理請求錯誤"""
        self.logger.error(f"Request {request_id} failed: {error_msg}")
        
        # 更新統計
        if config and config.model:
            self._update_model_statistics(config.model, False, response_time)
        
        # 自動重試邏輯
        if (self.auto_retry_enabled and config and 
            config.max_retries > 0 and 
            self.retry_counters.get(request_id, 0) < config.max_retries):
            
            retry_count = self.retry_counters.get(request_id, 0) + 1
            self.retry_counters[request_id] = retry_count
            
            # 延遲重試（指數退避）
            delay = min(2 ** (retry_count - 1), 10)  # 最大10秒
            
            def retry_request():
                time.sleep(delay)
                priority = -config.metadata.get('priority', 0)  # 負數表示高優先級
                self.request_queue.put((priority, request_id, config))
            
            threading.Thread(target=retry_request, daemon=True).start()
            
            self.logger.info(f"Retrying request {request_id}, attempt {retry_count}")
            return
        
        # 發送錯誤回調
        if self.on_request_error:
            self.on_request_error(request_id, error_msg, self.retry_counters.get(request_id, 0))
        
        self.failed_requests += 1
    
    async def refresh_model_list(self) -> bool:
        """
        刷新可用模型列表
        
        Returns:
            是否成功獲取模型列表
        """
        if not self.is_service_available:
            return False
        
        self.logger.info("Refreshing model list...")
        
        try:
            async with aiohttp.ClientSession() as session:
                async with session.get(
                    f"{self.ollama_url}/api/tags",
                    timeout=aiohttp.ClientTimeout(total=10)
                ) as response:
                    
                    if response.status != 200:
                        self.logger.error(f"Failed to get model list: HTTP {response.status}")
                        return False
                    
                    data = await response.json()
                    models = data.get('models', [])
                    
                    self.available_models.clear()
                    
                    for model_data in models:
                        info = ModelInfo(
                            name=model_data.get('name', ''),
                            size=model_data.get('size', 0),
                            digest=model_data.get('digest', ''),
                            modified_at=self._parse_datetime(model_data.get('modified_at')),
                            is_available=True
                        )
                        
                        details = model_data.get('details', {})
                        info.family = details.get('family', '')
                        info.parameter_size = details.get('parameter_size', '')
                        info.quantization_level = details.get('quantization_level', '')
                        
                        self.available_models[info.name] = info
                        
                        self.logger.info(
                            f"Found model: {info.name} "
                            f"Family: {info.family} "
                            f"Size: {info.size / 1024 / 1024:.1f}MB"
                        )
                    
                    self.logger.info(f"Model list refreshed, found {len(self.available_models)} models")
                    return True
                    
        except Exception as e:
            self.logger.error(f"Failed to refresh model list: {e}")
            return False
    
    def select_best_model(self, scenario: ScenarioType) -> Optional[str]:
        """
        根據場景選擇最佳模型
        
        Args:
            scenario: 使用場景
            
        Returns:
            推薦的模型名稱
        """
        if not self.available_models:
            return None
        
        # 基於場景的模型選擇策略
        preferred_models = []
        
        if scenario == ScenarioType.CODE_GENERATION:
            preferred_models = ["deepseek-coder:latest", "llama3:latest", "mistral:latest"]
        elif scenario == ScenarioType.TECHNICAL_SUPPORT:
            preferred_models = ["llama3:latest", "mistral:latest", "qwen2:latest"]
        elif scenario == ScenarioType.GAME_NARRATIVE:
            preferred_models = ["gemma:latest", "llama3:latest", "mistral:latest"]
        elif scenario == ScenarioType.DATA_ANALYSIS:
            preferred_models = ["qwen2:latest", "llama3:latest", "mistral:latest"]
        elif scenario == ScenarioType.TRANSLATION:
            preferred_models = ["qwen2:latest", "gemma:latest", "llama3:latest"]
        elif scenario == ScenarioType.CREATIVE_WRITING:
            preferred_models = ["gemma:latest", "llama3:latest", "mistral:latest"]
        elif scenario == ScenarioType.DEBUGGING:
            preferred_models = ["deepseek-coder:latest", "llama3:latest", "phi3:latest"]
        elif scenario == ScenarioType.QUESTION_ANSWERING:
            preferred_models = ["llama3:latest", "qwen2:latest", "mistral:latest"]
        elif scenario == ScenarioType.SUMMARIZATION:
            preferred_models = ["phi3:latest", "qwen2:latest", "gemma:latest"]
        else:  # GENERAL_CHAT
            preferred_models = ["llama3:latest", "mistral:latest", "phi3:latest", "gemma:latest", "qwen2:latest"]
        
        # 查找可用的首選模型
        for model_name in preferred_models:
            if (model_name in self.available_models and 
                self.available_models[model_name].is_available and 
                self.is_model_healthy(model_name)):
                return model_name
        
        # 如果沒有首選模型，使用性能最好的可用模型
        best_model = None
        best_score = -1.0
        
        for name, info in self.available_models.items():
            if not info.is_available or not self.is_model_healthy(name):
                continue
            
            # 計算模型評分（基於成功率和響應時間）
            success_rate = info.success_rate if info.success_count + info.error_count > 0 else 0.5
            response_score = 1000.0 / info.avg_response_time if info.avg_response_time > 0 else 1.0
            
            score = success_rate * 0.7 + response_score * 0.3
            
            if score > best_score:
                best_score = score
                best_model = name
        
        if best_model is None and self.available_models:
            # 最後備選：返回第一個可用模型
            for name, info in self.available_models.items():
                if info.is_available:
                    best_model = name
                    break
        
        self.logger.info(f"Selected model for scenario {scenario.value}: {best_model}")
        return best_model
    
    def is_model_healthy(self, model_name: str) -> bool:
        """
        檢查模型是否健康
        
        Args:
            model_name: 模型名稱
            
        Returns:
            模型是否健康
        """
        if model_name not in self.available_models:
            return False
        
        info = self.available_models[model_name]
        if not info.is_available:
            return False
        
        # 如果沒有統計數據，認為是健康的
        total_requests = info.success_count + info.error_count
        if total_requests == 0:
            return True
        
        # 成功率大於50%且響應時間合理
        success_rate = info.success_rate
        response_time_ok = info.avg_response_time < 30000  # 30秒內響應
        
        return success_rate > 0.5 and response_time_ok
    
    def generate_sync(self, config: LLMRequestConfig) -> LLMResponse:
        """
        同步LLM請求
        
        Args:
            config: 請求配置
            
        Returns:
            響應結果
        """
        if not self.is_initialized or not self.is_service_available:
            return LLMResponse(
                success=False,
                error_message="LLM service not available"
            )
        
        # 創建異步循環
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        
        try:
            return loop.run_until_complete(self._generate_async(config))
        finally:
            loop.close()
    
    async def _generate_async(self, config: LLMRequestConfig) -> LLMResponse:
        """異步生成響應"""
        start_time = time.time()
        
        # 選擇模型
        model_name = config.model or self.select_best_model(config.scenario)
        if not model_name:
            return LLMResponse(
                success=False,
                error_message="No suitable model available"
            )
        
        # 構建請求
        payload = self._build_request_payload(config)
        payload["model"] = model_name
        
        try:
            async with aiohttp.ClientSession() as session:
                timeout = aiohttp.ClientTimeout(total=config.timeout_ms / 1000)
                
                async with session.post(
                    f"{self.ollama_url}/api/generate",
                    json=payload,
                    timeout=timeout
                ) as response:
                    
                    response_time = int((time.time() - start_time) * 1000)
                    
                    if response.status != 200:
                        error_msg = await response.text()
                        self._update_model_statistics(model_name, False, response_time)
                        return LLMResponse(
                            success=False,
                            error_message=f"HTTP {response.status}: {error_msg}",
                            model=model_name,
                            response_time_ms=response_time,
                            timestamp=datetime.now()
                        )
                    
                    data = await response.json()
                    content = data.get('response', '')
                    
                    if content:
                        llm_response = LLMResponse(
                            success=True,
                            content=content,
                            model=model_name,
                            response_time_ms=response_time,
                            timestamp=datetime.now(),
                            raw_response=data
                        )
                        
                        self._update_model_statistics(model_name, True, response_time)
                        self.successful_requests += 1
                    else:
                        llm_response = LLMResponse(
                            success=False,
                            error_message=data.get('error', 'Empty response'),
                            model=model_name,
                            response_time_ms=response_time,
                            timestamp=datetime.now(),
                            raw_response=data
                        )
                        
                        self._update_model_statistics(model_name, False, response_time)
                        self.failed_requests += 1
                    
                    self.total_requests += 1
                    return llm_response
                    
        except Exception as e:
            response_time = int((time.time() - start_time) * 1000)
            self._update_model_statistics(model_name, False, response_time)
            self.failed_requests += 1
            self.total_requests += 1
            
            return LLMResponse(
                success=False,
                error_message=str(e),
                model=model_name,
                response_time_ms=response_time,
                timestamp=datetime.now()
            )
    
    def generate_async(self, config: LLMRequestConfig, request_id: Optional[str] = None) -> str:
        """
        異步LLM請求
        
        Args:
            config: 請求配置
            request_id: 請求ID（用於追蹤）
            
        Returns:
            請求ID
        """
        if request_id is None:
            request_id = self._generate_request_id()
        
        # 添加到請求隊列
        priority = -config.metadata.get('priority', 0)  # 負數表示高優先級
        self.request_queue.put((priority, request_id, config))
        
        self.logger.info(f"Async request queued: {request_id}")
        return request_id
    
    def quick_chat(self, message: str, system_prompt: str = "", model_hint: str = "") -> str:
        """
        快速對話請求
        
        Args:
            message: 用戶消息
            system_prompt: 系統提示詞
            model_hint: 模型提示（空則自動選擇）
            
        Returns:
            響應內容
        """
        if not system_prompt:
            system_prompt = "You are a helpful AI assistant in the RANOnline game system. Please provide helpful and accurate responses."
        
        config = LLMRequestConfig(
            prompt=message,
            system_prompt=system_prompt,
            model=model_hint,
            scenario=ScenarioType.GENERAL_CHAT,
            stream=False
        )
        
        response = self.generate_sync(config)
        return response.content if response.success else f"Error: {response.error_message}"
    
    def generate_code(self, requirement: str, language: str = "cpp") -> str:
        """
        代碼生成請求
        
        Args:
            requirement: 需求描述
            language: 編程語言
            
        Returns:
            生成的代碼
        """
        config = LLMRequestConfig(
            prompt=f"Generate {language} code for the following requirement:\n{requirement}",
            system_prompt=(
                f"You are an expert {language} programmer. Generate clean, well-commented, "
                f"and efficient code. Include necessary headers and follow best practices "
                f"for {language} development."
            ),
            scenario=ScenarioType.CODE_GENERATION,
            stream=False
        )
        
        response = self.generate_sync(config)
        return response.content if response.success else f"Error: {response.error_message}"
    
    def answer_technical_question(self, question: str, context: str = "") -> str:
        """
        技術問題解答
        
        Args:
            question: 技術問題
            context: 上下文信息
            
        Returns:
            解答內容
        """
        prompt = question if not context else f"Context: {context}\n\nQuestion: {question}"
        
        config = LLMRequestConfig(
            prompt=prompt,
            system_prompt=(
                "You are a technical expert assistant. Provide detailed, accurate, and practical "
                "answers to technical questions. Include examples and explanations when appropriate."
            ),
            scenario=ScenarioType.TECHNICAL_SUPPORT,
            stream=False
        )
        
        response = self.generate_sync(config)
        return response.content if response.success else f"Error: {response.error_message}"
    
    def get_queue_status(self) -> Dict[str, Any]:
        """
        獲取隊列狀態
        
        Returns:
            隊列信息
        """
        return {
            "queue_size": self.request_queue.qsize(),
            "active_requests": len(self.active_requests),
            "max_concurrent": self.max_concurrent_requests,
            "total_requests": self.total_requests,
            "successful_requests": self.successful_requests,
            "failed_requests": self.failed_requests,
            "model_statistics": {
                name: {
                    "success_count": info.success_count,
                    "error_count": info.error_count,
                    "avg_response_time": info.avg_response_time,
                    "success_rate": info.success_rate,
                    "is_available": info.is_available
                }
                for name, info in self.available_models.items()
            }
        }
    
    def load_configuration(self, config_path: str = "ollama_config.json"):
        """
        載入配置文件
        
        Args:
            config_path: 配置文件路徑
        """
        config_dir = Path.home() / ".ranonline"
        config_file = config_dir / config_path
        
        if config_file.exists():
            try:
                with open(config_file, 'r', encoding='utf-8') as f:
                    config = json.load(f)
                
                self.global_options = config.get("global_options", {})
                self.model_selection_strategy = config.get("model_selection_strategy", "balanced")
                self.auto_retry_enabled = config.get("auto_retry_enabled", True)
                self.max_concurrent_requests = config.get("max_concurrent_requests", 3)
                
                self.logger.info(f"Configuration loaded from: {config_file}")
            except Exception as e:
                self.logger.error(f"Failed to load configuration: {e}")
        else:
            self.logger.info("No configuration file found, using defaults")
    
    def save_configuration(self, config_path: str = "ollama_config.json"):
        """
        保存配置文件
        
        Args:
            config_path: 配置文件路徑
        """
        config_dir = Path.home() / ".ranonline"
        config_dir.mkdir(parents=True, exist_ok=True)
        config_file = config_dir / config_path
        
        config = {
            "global_options": self.global_options,
            "model_selection_strategy": self.model_selection_strategy,
            "auto_retry_enabled": self.auto_retry_enabled,
            "max_concurrent_requests": self.max_concurrent_requests
        }
        
        try:
            with open(config_file, 'w', encoding='utf-8') as f:
                json.dump(config, f, indent=2, ensure_ascii=False)
            
            self.logger.info(f"Configuration saved to: {config_file}")
        except Exception as e:
            self.logger.error(f"Failed to save configuration: {e}")
    
    def stop(self):
        """停止LLM管理器"""
        self._stop_event.set()
        if self._queue_thread and self._queue_thread.is_alive():
            self._queue_thread.join(timeout=5)
        
        self.logger.info("Ollama LLM Manager stopped")
    
    def _build_request_payload(self, config: LLMRequestConfig) -> Dict[str, Any]:
        """構建請求負載"""
        prompt = config.prompt
        if config.system_prompt:
            prompt = f"System: {config.system_prompt}\n\nUser: {config.prompt}"
        
        payload = {
            "prompt": prompt,
            "stream": config.stream
        }
        
        # 合併全域選項和請求特定選項
        options = {**self.global_options, **config.options}
        if options:
            payload["options"] = options
        
        return payload
    
    def _update_model_statistics(self, model_name: str, success: bool, response_time: int):
        """更新模型統計信息"""
        if model_name in self.available_models:
            info = self.available_models[model_name]
            
            if success:
                info.success_count += 1
                # 更新平均響應時間
                if info.avg_response_time == 0.0:
                    info.avg_response_time = response_time
                else:
                    info.avg_response_time = (info.avg_response_time + response_time) / 2.0
            else:
                info.error_count += 1
    
    def _log_request(self, request_id: str, response: LLMResponse):
        """記錄請求日誌"""
        self.logger.info(
            f"Request {request_id}: "
            f"Model={response.model} "
            f"Success={response.success} "
            f"ResponseTime={response.response_time_ms}ms "
            f"ContentLength={len(response.content)}"
        )
    
    def _generate_request_id(self) -> str:
        """生成請求ID"""
        timestamp = int(time.time() * 1000)
        unique_id = str(uuid.uuid4())[:8]
        return f"req_{timestamp}_{unique_id}"
    
    def _parse_datetime(self, dt_string: Optional[str]) -> Optional[datetime]:
        """解析日期時間字符串"""
        if not dt_string:
            return None
        
        try:
            # 嘗試解析ISO格式
            return datetime.fromisoformat(dt_string.replace('Z', '+00:00'))
        except Exception:
            return None


# 工廠函數
def create_ollama_manager(ollama_url: str = "http://localhost:11434") -> OllamaLLMManager:
    """
    創建Ollama LLM管理器實例
    
    Args:
        ollama_url: Ollama服務器URL
        
    Returns:
        LLM管理器實例
    """
    return OllamaLLMManager(ollama_url)


# 示例用法
if __name__ == "__main__":
    async def main():
        # 創建管理器
        manager = create_ollama_manager()
        
        # 初始化
        if await manager.initialize():
            print("LLM Manager initialized successfully")
            
            # 快速對話測試
            response = manager.quick_chat("Hello, how are you?")
            print(f"Response: {response}")
            
            # 代碼生成測試
            code = manager.generate_code("Create a hello world function", "python")
            print(f"Generated code:\n{code}")
            
            # 技術問題解答測試
            answer = manager.answer_technical_question("What is the difference between stack and heap memory?")
            print(f"Technical answer: {answer}")
            
            # 獲取狀態
            status = manager.get_queue_status()
            print(f"Queue status: {json.dumps(status, indent=2)}")
        else:
            print("Failed to initialize LLM Manager")
        
        # 停止管理器
        manager.stop()
    
    # 運行示例
    asyncio.run(main())
