import sys

import gym
import numpy as np
import gym.spaces
import cv2

from gym.envs.classic_control import rendering

cimport numpy as np

cdef int WIDTH = 25
cdef int HEIGHT = 25
cdef int MAX_STEPS = 499
cdef int RESTORE_POINTS = 10
cdef np.ndarray BLUE = np.array([255, 0, 0])
cdef np.ndarray GREEN = np.array([0, 255, 0])
cdef np.ndarray RED = np.array([0, 0, 255])
cdef np.ndarray BLACK = np.array([0, 0, 0])
cdef list NEXTS = [
    np.array([-1, 0]),
    np.array([0, 1]),
    np.array([1, 0]),
    np.array([0, -1])
]

cdef class MapRootEnvCy:
    """経路探索"""
    metadata = {'render.modes': ['human', 'ansi']}
    FIELD_TYPES = [
        'S',
        'G',
        'R',
        'N'
    ]
    

    cdef int steps
    cdef public int[3] input_shape
    cdef int done
    cdef np.ndarray pos, start, goal
    cdef np.ndarray map
    cdef list trail


    def __init__(self):
        self.steps = 0
        self.done = 0

        # 実際の環境からshapeを取得する
        cdef int width = WIDTH
        cdef int height = HEIGHT
        cdef int channel = len(self.FIELD_TYPES)
        self.input_shape[0] = height
        self.input_shape[1] = width
        self.input_shape[2] = channel

        # 報酬の最大最小
        self.reset()
        pass

    cdef random_pos(self):
        cdef int x = np.random.randint(WIDTH)
        cdef int y = np.random.randint(HEIGHT)
        return x, y

    cdef generate_map(self):
        cdef np.ndarray[np.int_t, ndim=3] root_map = np.zeros(self.input_shape, dtype=int)

        cdef int s_x, s_y, g_x, g_y, r_x, r_y
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
        for _ in range(RESTORE_POINTS):
            r_x, r_y = self.random_pos()
            root_map[r_y][r_x][2] = 1
        return np.array([s_y, s_x]), np.array([s_y, s_x]), np.array([g_y, g_x]), root_map

    def map_to_image(self):
        cdef np.ndarray[np.int_t, ndim=3] img = np.ones([HEIGHT, WIDTH, 3], dtype=np.int) * 255
        # 補給場所は緑
        img[self.map[:, :, 2] == 1] = GREEN
        # 開始地点は青
        img[self.map[:, :, 0] == 1] = BLUE
        # 終了地点は赤
        img[self.map[:, :, 1] == 1] = RED
        # 現在位置は黒
        img[self.map[:, :, 3] == 1] = BLACK
        return img.astype(np.uint8)

    def reset(self):
        self.steps = 0
        self.done = 0
        self.pos, self.start, self.goal, self.map = self.generate_map()
        self.trail = [self.pos]
        return self.map

    cdef next_pos(self, int index, np.ndarray[np.int_t, ndim=1] pos):
        cdef np.ndarray[np.int_t, ndim=1] next_position = pos + NEXTS[index]
        # 画面外に出ていないかチェック
        next_position[0] = np.clip(next_position[0], 0, HEIGHT-1)
        next_position[1] = np.clip(next_position[1], 0, WIDTH-1)
        return next_position

    cdef rewards(self, np.ndarray[np.int_t, ndim=1] pos, np.ndarray[np.int_t, ndim=1] next_pos):
        cdef float rewards = 0
        cdef np.ndarray[np.int_t, ndim=1] state = self.map[next_pos[0]][next_pos[1]]
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
        cdef np.ndarray[np.int_t, ndim=1] diff = next_pos - pos
        if np.linalg.norm(diff) < 1:
            # print(diff)
            # 移動していなければマイナス
            rewards -= 0.5

        # 時間経過で少しマイナス
        rewards -= 0.2

        return rewards

    cdef update_map(self, np.ndarray[np.int_t, ndim=1] pos, np.ndarray[np.int_t, ndim=1] next_pos):
        # 移動
        self.map[pos[0]][pos[1]][3] = 0
        self.map[next_pos[0]][next_pos[1]][3] = 1

    cdef _is_done(self, np.ndarray[np.int_t, ndim=1] pos):
        if np.all(pos == self.goal):
            # ゴールに着いたら
            return 1
        elif self.steps > MAX_STEPS:
            # 制限時間を超えたら
            return 1
        else:
            return 0

    cdef action_step(self, int action):
        """actionは数字"""
        self.steps += 1
        # 次の位置を確認
        cdef np.ndarray[np.int_t, ndim=1] next_pos = self.next_pos(action, self.pos)
        # 報酬を計算
        cdef float reward = self.rewards(self.pos, next_pos)
        # マップを更新
        self.update_map(self.pos, next_pos)

        self.pos = next_pos
        self.trail.append(self.pos)
        # 画像データの取得
        cdef np.ndarray[np.int_t, ndim=3] observation = self.map

        # 終了したかどうか
        self.done = self._is_done(self.pos)
        return observation, reward, bool(self.done), {}

    def step(self, action: int):
        return self.action_step(action)

    def render(self, mode='human', close=False):
        img = self.map_to_image()
        img = cv2.resize(
            img, (img.shape[1]*4, img.shape[0]*4), interpolation=cv2.INTER_NEAREST)
        cv2.imshow('image', img)

    def render_trailed_image(self):
        img = self.map_to_image()
        for y, x in self.trail:
            img[y][x] = BLACK
        img = cv2.resize(
            img, (img.shape[1]*4, img.shape[0]*4), interpolation=cv2.INTER_NEAREST)
        cv2.imshow('image', img)

    def _close(self):
        cv2.destroyAllWindows()

    def _seed(self, seed=None):
        pass
    
    def get_trail(self):
        return self.trail

class MapRootEnv(gym.Env):
    SCALE=3
    def __init__(self):
        self.env = MapRootEnvCy()
        self.input_shape = self.env.input_shape
        self.input_shape[0] *= self.SCALE
        self.input_shape[1] *= self.SCALE
        # 行動の個数
        self.action_space = gym.spaces.Discrete(
            4)
                # 画像データの情報（ピクセルの最大、最小）
        self.observation_space = gym.spaces.Box(
            low=0,
            high=len(self.env.FIELD_TYPES),
            shape=self.input_shape
        )
        
        self.reward_range = [-1.7, 11.]
    
    def reset(self):
        obs = self.env.reset()
        obs = cv2.resize(
            obs, (obs.shape[1]*self.SCALE, obs.shape[0]*self.SCALE), interpolation=cv2.INTER_NEAREST)
        obs = obs.transpose(2, 0, 1).astype(np.float32)
        return obs
    
    def step(self, action):
        # もとのマップをSCALE倍のサイズにして返す
        obs, reward, done, tmp = self.env.step(action)
        obs = cv2.resize(
            obs, (obs.shape[1]*self.SCALE, obs.shape[0]*self.SCALE), interpolation=cv2.INTER_NEAREST)
        obs = obs.transpose(2, 0, 1).astype(np.float32)

        return obs, reward, done, tmp
    
    def render(self, mode='human', close=False):
        img = self.env.map_to_image()
        img = cv2.resize(
            img, (img.shape[1]*self.SCALE*4, img.shape[0]*self.SCALE*4), interpolation=cv2.INTER_NEAREST)
        cv2.imshow('image', img)
    
    def render_trailed_image(self):
        img = self.env.map_to_image()
        for y, x in self.env.get_trail():
            img[y][x] = (img[y][x] + BLACK) / 2
        img = cv2.resize(
            img, (img.shape[1]*self.SCALE*4, img.shape[0]*self.SCALE*4), interpolation=cv2.INTER_NEAREST)
        cv2.imshow('image', img)
    
    def _close(self):
        self.env._close()
    
    def _seed(self, seed=None):
        self.env._seed(seed)