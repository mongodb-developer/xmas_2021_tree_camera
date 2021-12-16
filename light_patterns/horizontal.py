import json
from requests import post,get
import random

with open('data.txt', 'r') as infile:
    positions = json.loads(infile.read())

print(positions)

sorted_positions = sorted(positions, key=lambda k: k['x'])


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

state = [{"r":0,"g":0,"b":0}]*50
for x in range(50):
    state[sorted_positions[x]['n']] = ({"r":roff+x*rmul, "g":goff+x*gmul, "b": boff+x*bmul})

url = 'https://data.mongodb-api.com/app/xmastree-lpeci/endpoint/lights'
body = {"state": state}
headers = {'content-type': 'application/json'}

r = post(url, data=json.dumps(body), headers=headers)

if r.status_code == 200:
    print('John\'s Xmas tree was successfully randomized!')
    print(state)
else:
    print(f"Nice try! {r.status_code}") 