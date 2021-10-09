import time
import mini_asyncio

from termcolor import colored


async def hello_1():
  while True:
    before = time.time()
    await mini_asyncio.sleep(0.1)
    print(colored("Aww! 1 Wakup ", 'red'), time.time() - before)


async def hello_2():
  while True:
    before = time.time()
    await mini_asyncio.sleep(2)
    print(colored("Yay! 2 Wakup ", 'green'), time.time() - before)


async def blocking():
  print("Hello")
  await mini_asyncio.sleep(1)


async def one_two():
  while True:
    print("A")
    await mini_asyncio.sleep(1)
    print("B")
    await mini_asyncio.sleep(1)
    print("C")
    await mini_asyncio.sleep(1)

    # Create a new task for 'blocking()' and wait for that task to complete
    # (pointless could directly await -- but a test)
    await mini_asyncio.get_event_loop().create_task(blocking())


if __name__ == '__main__':
  loop = mini_asyncio.get_event_loop()
  mini_asyncio.ensure_future(hello_1())
  mini_asyncio.ensure_future(hello_2())
  mini_asyncio.ensure_future(one_two())
  loop.run_forever()

  # loop.run_until_complete(hello_1())
