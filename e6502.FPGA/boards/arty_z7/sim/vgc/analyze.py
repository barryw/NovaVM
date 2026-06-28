import sys
fn=sys.argv[1] if len(sys.argv)>1 else "vgc.ppm"
f=open(fn,"rb"); assert f.readline().strip()==b"P6"
w,h=map(int,f.readline().split()); f.readline(); d=f.read()
def px(x,y): o=(y*w+x)*3; return (d[o],d[o+1],d[o+2])
def edges(x):
    out=[];prev=None
    for y in range(h):
        c=px(x,y)
        if c!=prev: out.append(y);prev=c
    return [y for y in out if 40<=y<=300]
b=edges(8); a=edges(360)
print(f"{fn}: border(x=8) first edges {b[:8]}")
print(f"{fn}: active(x=360) first edges {a[:8]}")
n=min(len(a),len(b)); off=[b[i]-a[i] for i in range(n)]
print(f"border - active per band = {off[:8]}")
