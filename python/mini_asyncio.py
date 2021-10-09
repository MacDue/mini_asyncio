"""
A mini toy asyncio that only handles the happy path
"""
import heapq
import time
from collections import deque
from collections import namedtuple


Timer = namedtuple('Timer', ['when', 'callback'])


class Future:
  def __init__(self, loop):
    self._result = None
    self._done = False
    self._blocking = False
    self._loop = loop
    self._done_cbs = []

  def _schedule_callbacks(self):
    for callback in self._done_cbs:
      self._loop.call_soon(callback, self)

  def add_done_callback(self, callback):
    if not self._done:
      self._done_cbs.append(callback)
    else:
      self._loop.call_soon(callback, self)

  def set_done(self):
    self._done = True
    self._schedule_callbacks()

  def set_result(self, result):
    self._result = result
    self.set_done()

  @property
  def pending(self):
    return not self._done

  def __iter__(self):
    while not self._done:
      self._blocking = True
      yield self # pending future
    return self._result

  __await__ = __iter__


class Task(Future):

  def __init__(self, coro, loop):
    super().__init__(loop=loop)
    self._coro = coro
    self._loop = loop
    self._loop.call_soon(self._step)

  def _step(self):
    try:
      result = self._coro.send(None)
    except StopIteration as ret:
      self.set_result(ret.value)
      return

    if result is None:
      # give up the loop or a step (something else can run)
      self._loop.call_soon(self._step)
    elif isinstance(result, Future):
      if result._blocking:
        # this task is blocked on a future
        result.add_done_callback(self._wakeup)
    else:
      raise RuntimeError('coro yielded unexpected result')

  def _wakeup(self, future):
    self._step()


def _run_until_complete_cb(future):
  future._loop.stop()


class EventLoop:
  def __init__(self):
    self._ready_cbs = deque()
    self._timer_cbs_heap = []
    self._stopping = False

  def stop(self):
    self._stopping = True

  def call_soon(self, callback, *args, **kwargs):
    self._ready_cbs.append(lambda: callback(*args, **kwargs))

  def call_later(self, delay, callback, *args, **kwargs):
    heapq.heappush(self._timer_cbs_heap, Timer(
      time.time() + delay, lambda: callback(*args, **kwargs)))

  def create_task(self, coro):
    return Task(coro, loop=self)

  def create_future(self):
    return Future(loop=self)

  def _run_once(self):
    timeout = (
      self._timer_cbs_heap[0].when - time.time()
      if len(self._timer_cbs_heap) > 0
      else 0)

    # Only sleep when there's no work that can be done now
    if len(self._ready_cbs) <= 0:
      timeout = max(0, timeout)
      time.sleep(timeout)

    while (
      len(self._timer_cbs_heap) > 0
      and self._timer_cbs_heap[0].when <= time.time()
    ):
      _, timer_cb = heapq.heappop(self._timer_cbs_heap)
      self._ready_cbs.append(timer_cb)

    num_todo = len(self._ready_cbs)
    for _ in range(num_todo):
      callback = self._ready_cbs.popleft()
      callback()

  def run_forever(self):
    while True:
      self._run_once()
      if self._stopping:
        break
    self._stopping = False

  def run_until_complete(self, coro):
    future = ensure_future(coro, self)
    future.add_done_callback(_run_until_complete_cb)
    self.run_forever()
    return future._result


_current_loop = None


def get_event_loop():
  global _current_loop
  if _current_loop is None:
    _current_loop = EventLoop()
  return _current_loop


class CoroWrapper:
  def __init__(self, gen):
    self.gen = gen

  def __await__(self):
    return self.gen


def coroutine(func):
  def wrapper(*args, **kwargs):
    return CoroWrapper(func(*args, **kwargs))
  return wrapper


@coroutine
def sleep(delay, loop=None):
  if loop is None:
    loop = get_event_loop()

  future = loop.create_future()
  loop.call_later(delay, future.set_done)

  yield from future


def ensure_future(coro, loop=None):
  if loop is None:
    loop = get_event_loop()
  return loop.create_task(coro)
