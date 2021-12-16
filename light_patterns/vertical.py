import json
import random
from requests import post

state = []

rmul = random.randint(-5, 5)
gmul = random.randint(-5, 5)
bmul = random.randint(-5, 5)
goff=0
boff=0
roff=0
if rmul < 0:
    roff = abs(rmul*50)
if gmul < 0:
    goff = abs(gmul*50)
if bmul < 0:
    boff = abs(bmul*50)


for x in range(50):
    state.append({"r":roff+x*rmul, "g":goff+x*gmul, "b": boff+x*bmul})

url = 'https://data.mongodb-api.com/app/xmastree-lpeci/endpoint/lights'
body = {"state": state}
headers = {'content-type': 'application/json'}

r = post(url, data=json.dumps(body), headers=headers)

if r.status_code == 200:
    print('John\'s Xmas tree was successfully randomized!')
    print(state)
else:
    print(f"Nice try! {r.status_code}") 
