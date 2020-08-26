import particles
import gym
import time
from tqdm import tqdm
import random
import math


def main():
    env_name = "Bnuts-v0"
    env = gym.make(env_name)
    action_size = env.max_diff * env.separate
    t = 0
    dt = 0.1
    action = 0
    for _ in tqdm(range(10000)):
        env.render()
        # 直接ランダムだと壁に合わせて動く前に壁が戻ってしまってうまく動かない
        # action = random.randrange(0, action_size)
        
        # 振動させると無難（もともとの使用）
        # action = action_size * math.sin(t)
        # t += dt

        # ちょっとずつランダムにずらすとそれなりに振動する
        # action += random.randrange(-10, 10)
        # action = min(max(action, 0), action_size - 1)

        # 階段状にランダムにしたものの普通にランダムにした場合と変わらなかった。
        action = random.randrange(0, env.max_diff+ 1) * env.separate
        # print(action)
        env.step(action)
        # time.sleep(0.001)


if __name__ == "__main__":
    main()
