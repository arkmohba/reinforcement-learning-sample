import sys

import gym
import numpy as np
import gym.spaces

import cv2


class BNutsEnv(gym.Env):
    metadata = {'render.modes': ['human', 'ansi']}
    MAX_STEPS = 100000

    def __init__(self):
        self.steps = 0
        # TODO 実際の環境から取得する
        width = 300
        height = 300
        channel = 3
        self.input_shape = [width, height, channel]

        # TODO 実際の環境から取得する
        self.max_diff = 5.0
        self.separate = 10  # 更に10倍に分割
        # 行動の個数
        self.action_space = gym.spaces.Discrete(
            int(self.max_diff*self.separate))

        # 画像データの情報
        self.observation_space = gym.spaces.Box(
            low=0,
            high=255,
            shape=self.input_shape
        )

        # 報酬の最大最小
        self.reward_range = [-height, height]
        self._reset()
        pass

    def _step(self, action):
        """actionは数字"""
        # TODO updated
        base = action / self.separate


        # TODO 画像データの取得
        observation = None

        # TODO 報酬の取得（ブラジルナッツの位置と他の最高値を取得して差分を計算する）
        bnats_y = 500
        other_min_y = 200
        reward = other_min_y - bnats_y

        # TODO 終了したかどうか
        self.done = self._is_done(bnats_y, other_min_y)
        observation, reward, self.done, {}
        pass

    def _reset(self):
        self.steps = 0
        self.done = False
        pass

    def _render(self, mode='human', close=False):
        # 画像データを取り出して表示
        # img: np.ndarray
        # cv2.imshow('image', img)
        pass

    def _is_done(self, bnats_y, other_max_y):
        if other_max_y > bnats_y:
            return True
        elif self.steps > self.MAX_STEPS:
            return True
    # def _close(self):   #     pass

    # def _seed(self, seed=None):
    #     pass
