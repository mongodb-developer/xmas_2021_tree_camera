import json
from requests import post,get
import random

with open('data.txt', 'r') as infile:
    positions = json.loads(infile.read())

print(positions)

sorted_positions = sorted(positions, key=lambda k: k['x'])


c1={"r":0,"g":0,"b":200}
c2={"r":255,"g":255,"b":255}
c3={"r":200,"g":0,"b":0}

state = [{"r":0,"g":0,"b":0}]*50
for x in range(50):
    c=c1
    if(x>10):
        c=c2
    if(x>25):
        c=c3
    state[sorted_positions[x]['n']] = c

url = 'https://data.mongodb-api.com/app/xmastree-lpeci/endpoint/lights'
body = {"state": state}
headers = {'content-type': 'application/json'}

r = post(url, data=json.dumps(body), headers=headers)

if r.status_code == 200:
    print('John\'s Xmas tree was successfully randomized!')
    print(state)
else:
    print(f"Nice try! {r.status_code}") 