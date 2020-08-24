from gym.envs.registration import register

register(
    id='Myenv-v0',
    entry_point='myenv.env:MyEnv'
)
print("registerd!")