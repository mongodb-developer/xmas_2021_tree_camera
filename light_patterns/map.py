import json
from requests import post,get
import time 
from PIL import Image, ImageChops
import io


state = []
data = []
url = 'https://data.mongodb-api.com/app/xmastree-lpeci/endpoint/lights'

def brightest_point(image):
    # Find the brightest pixel
    max_brightness = 0
    x, y = 0, 0
    for i in range(image.size[0]):
        for j in range(image.size[1]):
            if sum(image.getpixel((i,j))) > max_brightness:
                max_brightness = sum(image.getpixel((i,j)))
                x, y = i, j
    return x, y

def process_image(n,i):
    image = Image.open(io.BytesIO(i.content))
    difference = ImageChops.difference(image, off_image)
    difference.save(F"lamp_{n}.png")
    x, y = brightest_point(difference)
    data.append({"n":n,"x":x,"y":y})
    print(f"{n} {x} {y}")

#Get base image
state = [{"r":0,"g":0,"b":0}]*50
body = {"state": state}
headers = {'content-type': 'application/json'}
r = post(url, data=json.dumps(body), headers=headers)
time.sleep(3); #Wait for Camera to get it
i = get('https://data.mongodb-api.com/app/xmastree-lpeci/endpoint/image')
off_image = Image.open(io.BytesIO(i.content))
off_image.save("off.png")

for x in range(50):
    state = [{"r":0,"g":0,"b":0}]*50
    state[x] = {"r":255,"g":255,"b":255}

   
    body = {"state": state}
    headers = {'content-type': 'application/json'}

    r = post(url, data=json.dumps(body), headers=headers)

    if r.status_code == 200:
        print('John\'s Xmas tree was successfully randomized!')
        print(x)
        time.sleep(4)
        i = get('https://data.mongodb-api.com/app/xmastree-lpeci/endpoint/image')
        process_image(x,i)
    else:
        print(f"Nice try! {r.status_code}") 
    time.sleep(1);

with open('data.txt', 'w') as outfile:
    json.dump(data, outfile)
