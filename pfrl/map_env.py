import sys

import gym
import numpy as np
import gym.spaces
import cv2

from gym.envs.classic_control import rendering


class MapRootEnv(gym.Env):
    """経路探索"""
    metadata = {'render.modes': ['human', 'ansi']}
    FIELD_TYPES = [
        'S',
        'G',
        'R',
        'N'
    ]
    WIDTH = 50
    HEIGHT = 50
    MAX_STEPS = 499
    RESTORE_POINTS = 10
    BLUE = np.array([255, 0, 0])
    GREEN = np.array([0, 255, 0])
    RED = np.array([0, 0, 255])
    BLACK = np.array([0, 0, 0])
    NEXTS = [
        np.array([-1, 0]),
        np.array([0, 1]),
        np.array([1, 0]),
        np.array([0, -1])
    ]

    def __init__(self):
        self.steps = 0
        self.viewer = rendering.SimpleImageViewer()
        self.map: np.ndarray

        # 実際の環境からshapeを取得する
        width = self.WIDTH
        height = self.HEIGHT
        channel = len(self.FIELD_TYPES)
        self.input_shape = [height, width, channel]

        # 行動の個数
        self.action_space = gym.spaces.Discrete(
            4)

        # 画像データの情報（ピクセルの最大、最小）
        self.observation_space = gym.spaces.Box(
            low=0,
            high=len(self.FIELD_TYPES),
            shape=self.input_shape
        )

        # 報酬の最大最小
        self.reward_range = [-1.7, 11.]
        self.reset()
        pass

    def random_pos(self):
        x = np.random.randint(self.WIDTH)
        y = np.random.randint(self.HEIGHT)
        return x, y

    def generate_map(self):
        root_map = np.zeros(self.input_shape, dtype=int)

        s_x, s_y = self.random_pos()
        root_map[s_y][s_x][0] = 1  # 開始位置
        root_map[s_y][s_x][3] = 1  # 現在位置

        while True:
            g_x, g_y = self.random_pos()
            if g_x == s_x and g_y == s_y:  # 終了位置
                continue
            root_map[g_y][g_x][1] = 1
            break

        # 補給場所
        for _ in range(self.RESTORE_POINTS):
            r_x, r_y = self.random_pos()
            root_map[r_y][r_x][2] = 1
        return np.array([s_y, s_x]), np.array([s_y, s_x]), np.array([g_y, g_x]), root_map

    def map_to_image(self, root_map):
        img = np.ones([self.HEIGHT, self.WIDTH, 3]) * 255
        # 補給場所は緑
        img[root_map[:, :, 2] == 1] = self.GREEN
        # 開始地点は青
        img[root_map[:, :, 0] == 1] = self.BLUE
        # 終了地点は赤
        img[root_map[:, :, 1] == 1] = self.RED
        # 現在位置は黒
        img[root_map[:, :, 3] == 1] = self.BLACK
        return img

    def reset(self):
        self.steps = 0
        self.done = False
        self.pos, self.start, self.goal, self.map = self.generate_map()
        self.trail = [self.pos]
        return self.map.transpose(2, 0, 1).astype(np.float32)

    def next_pos(self, index: int, pos: np.ndarray):
        next_pos = pos + self.NEXTS[index]
        # 画面外に出ていないかチェック
        next_pos[0] = np.clip(next_pos[0], 0, self.HEIGHT-1)
        next_pos[1] = np.clip(next_pos[1], 0, self.WIDTH-1)
        return next_pos

    def rewards(self, pos, next_pos):
        rewards = 0
        state = self.map[next_pos[0]][next_pos[1]]
        if state[0] == 1:
            # 開始地点に戻ってきたらマイナス
            rewards -= 1
        elif state[1] == 1:
            # 終了地点に戻ってきたらプラス
            rewards += 100
        if state[2] == 1:
            # 補給地点に来れたらプラス
            # TODO 時間変動をつける
            rewards += 0.15
        if np.linalg.norm(self.goal - next_pos) < np.linalg.norm(self.goal - pos):
            # ゴールに近づいたらプラス
            rewards += 0.05

        # 移動のコストを追加
        # TODO できれば道の高低差をコストに入れたい
        diff = next_pos - pos
        if diff.sum() < 1:
            # 移動していなければマイナス
            rewards -= 0.5

        # 時間経過で少しマイナス
        rewards -= 0.2

        return rewards

    def update_map(self, pos, next_pos):
        # 移動
        self.map[pos[0]][pos[1]][3] = 0
        self.map[next_pos[0]][next_pos[1]][3] = 1

    def _is_done(self, pos):
        if np.all(pos == self.goal):
            # ゴールに着いたら
            return True
        elif self.steps > self.MAX_STEPS:
            # 制限時間を超えたら
            return True
        else:
            False

    def step(self, action):
        """actionは数字"""
        self.steps += 1
        # 次の位置を確認
        next_pos = self.next_pos(action, self.pos)
        # 報酬を計算
        reward = self.rewards(self.pos, next_pos)
        # マップを更新
        self.update_map(self.pos, next_pos)

        self.pos = next_pos
        self.trail.append(self.pos)
        # 画像データの取得
        observation = self.map

        # 終了したかどうか
        self.done = self._is_done(self.pos)
        return observation.transpose(2, 0, 1).astype(np.float32), reward, self.done, {}

    def render(self, mode='human', close=False):
        img = self.map_to_image(self.map)
        img = cv2.resize(
            img, (img.shape[1]*4, img.shape[0]*4), interpolation=cv2.INTER_NEAREST)
        cv2.imshow('image', img)

    def render_trailed_image(self):
        img = self.map_to_image(self.map)
        for y, x in self.trail:
            img[y][x] = self.BLACK
        img = cv2.resize(
            img, (img.shape[1]*4, img.shape[0]*4), interpolation=cv2.INTER_NEAREST)
        cv2.imshow('image', img)

    def _close(self):
        cv2.destroyAllWindows()

    def _seed(self, seed=None):
        pass
