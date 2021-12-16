import json
import random
from requests import post

state = []
for _ in range(50):
    state.append({"r": random.randint(0, 255), "g": random.randint(0, 255), "b": random.randint(0, 255)})

url = 'https://data.mongodb-api.com/app/xmastree-lpeci/endpoint/lights'
body = {"state": state}
headers = {'content-type': 'application/json'}

r = post(url, data=json.dumps(body), headers=headers)

if r.status_code == 200:
    print('John\'s Xmas tree was successfully randomized!')
else:
    print(f"Nice try! {r.status_code}") 
