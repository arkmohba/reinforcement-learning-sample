import particles
import gym
import time
from tqdm import tqdm
import random
import math
import cv2

def main():
    env_name = "Bnuts-v0"
    env = gym.make(env_name)
    action_size = env.max_diff * env.separate
    t = 0
    dt = 0.1
    action = 0

    frame_rate = 30  # フレームレート
    size = env.observation_space.shape[:2]  # 動画の画面サイズ
    size = (size[1], size[0])  # width, heightの順

    fmt = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')  # ファイル形式(ここではmp4)
    writer = cv2.VideoWriter('./output.mp4', fmt, frame_rate, size)  # ライター作成


    for _ in tqdm(range(1000)):
        env.render()
        # 直接ランダムだと壁に合わせて動く前に壁が戻ってしまってうまく動かない
        # action = random.randrange(0, action_size)
        
        # 振動させると無難（もともとの仕様）
        action = action_size * math.sin(t)
        t += dt

        # ちょっとずつランダムにずらすとそれなりに振動する
        # action += random.randrange(-10, 10)
        # action = min(max(action, 0), action_size - 1)

        # 階段状にランダムにしたものの普通にランダムにした場合と変わらなかった。
        # action = random.randrange(0, env.max_diff+ 1) * env.separate
        # print(action)
        obs, _, _, _ = env.step(action)
        writer.write(obs)  # 画像を1フレーム分として書き込み
        # time.sleep(0.001)

    writer.release()  # ファイルを閉じるs

if __name__ == "__main__":
    main()
