# coding:utf-8
from flask import Flask,render_template, flash, request, redirect, url_for, send_from_directory
import urllib
import numpy as np
from PIL import Image, ImageDraw
import networkx as nx
import os
from werkzeug import secure_filename
import matplotlib.pyplot as plt
import datetime
import subprocess

app = Flask(__name__)



UPLOAD_FOLDER = './uploads'
ALLOWED_EXTENSIONS = set(['jpg','png','gif'])
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['SECRET_KEY'] = os.urandom(24)

def allowed_file(filename):
    return '.' in filename and \
        filename.rsplit('.', 1)[1] in ALLOWED_EXTENSIONS



@app.route('/')
def index():
    name="yoko"
    pic1="/images/fig2.png"
    return render_template('form.html',title="Maze",name=name,pic=pic1)

@app.route('/confirm', methods = ['POST', 'GET'])
def show_img():
    if request.method == 'POST':
        if 'img_data' not in request.files:
            flash('No file part')
            return redirect(request.url)
        img_data=request.files['img_data']
        value=request.form["size"]
        if img_data and allowed_file(img_data.filename):
            print("kkkkkkkkkkkkkkkkkkkkkkkk")
            filename = secure_filename(img_data.filename)
            basedir = os.path.abspath(os.path.dirname(__file__))
            img_data.save(os.path.join(basedir,app.config['UPLOAD_FOLDER'], filename))
            MAIN_FILENAME = './uploads/' + filename
            if value=="1":
                size=10
            if value=="2":
                size=20
            if value=="3":
                size=30
            #print(img_file)
            im = np.array(Image.open(MAIN_FILENAME).convert('L').resize((size, size)))
            th=15
            im_bin_50 = (im > th) * 255
            #これで画像が二値化できた。
            Image.fromarray(np.uint8(im_bin_50)).save('7.png')

            #画像から得た行列を出力する

            #二値化した画像をもとにグラフを作成していく。
            G = nx.Graph()

            for i in range(size):
                for j in range(size):
                    if im_bin_50[i][j] == 0:
                        G.add_node((i,j))

            for i in range(size):
                for j in range(size-1):
                    if im_bin_50[i][j] == 0 and im_bin_50[i][j+1] == 0:
                        G.add_edge((i,j),(i,j+1))

            for i in range(size-1):
                for j in range(size):
                    if im_bin_50[i][j] == 0 and im_bin_50[i+1][j] == 0:
                        G.add_edge((i,j),(i+1,j))

            str1=list(G.nodes)
            str2=list(G.edges)
            sss=str(len(G.nodes))+"\n"
            for i in range(len(str1)):
                sss+=str(list(G.nodes)[i][0])+" "+str(list(G.nodes)[i][1])+"\n"

            sss+=str(len(G.edges))+"\n"

            for i in range(len(str2)):
                sss+=str(list(G.edges)[i][0][0])+" "+str(list(G.edges)[i][0][1])+" "+str(list(G.edges)[i][1][0])+" "+str(list(G.edges)[i][1][1])+"\n"


            with open("graph2.txt","w") as f:
                f.write(sss)

            print(sss)
            #print(len(G.edges))
            pos = {n: (n[0], n[1]) for n in G.nodes()}
            nx.draw_networkx_nodes(G, pos, node_size=10, alpha=1, node_color='red')
            nx.draw_networkx_edges(G, pos, font_size=10, label=1, edge_color="black", width=2)
            #plt.show()
            pos = {n: (n[0], n[1]) for n in G.nodes()}
            plt.savefig(os.path.join(basedir,app.config['UPLOAD_FOLDER'], filename))
            img_url = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            print(img_url)
            fname, ext = os.path.splitext(filename)
            return render_template("confirm.html",title="Maze",img_url=img_url,filename=filename,sss=sss)

@app.route('/show', methods = ['POST', 'GET'])
def show():
    #./may3.out < graph2.txt >mazelist3.py
    now=datetime.datetime.now()
    f1='mazelist_{0:%Y%m%d%H%M}'.format(now)
    proc = subprocess.run(["./may3.out < graph2.txt > mazelist3.py"],shell=True,stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    #list=proc.stdout.decode("utf8")
    from mazelist3 import mazelist
    images = []
    data=mazelist
    rows=len(data)
    cols=len(data[0])
    width = 200
    center = width // 2
    color_1 = (0, 0, 0)
    color_2 = (255, 255, 255)
    color_3 = (255, 0, 0)
    color_4 = (0, 255, 0)
    max_radius = int(center * 1.5)
    step = 8
    tile_size=10
    im = Image.new('RGB', (cols*tile_size, rows*tile_size), color_2)
    draw = ImageDraw.Draw(im)
    x=0
    y=0
    k=0
    for i in range(0, rows):
        y=i*tile_size
        y1=y+tile_size
        for j in range(0, cols):
            x=j*tile_size
            x1=x+tile_size
            if data[i][j]==0:
                colorthis=color_2
            if data[i][j]==1:
                colorthis=color_1
            if data[i][j]==2:
                colorthis=color_3
            if data[i][j]==3:
                colorthis=color_4
            draw.rectangle((x,y, x1, y1), fill=colorthis,outline=color_1)

    im.save('pillow_imagedraw.jpg', quality=95)
    #makemaze(mazelist)
    img_url2='pillow_imagedraw.jpg'
    return render_template("showmaze.html",title="Maze",img_url2=img_url2)



if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0',port=5005)
