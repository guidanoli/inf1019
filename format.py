import re
VERTICAL_LINE = '\u2503'
HORIZONTAL_LINE = '\u2501'
INTERSECTION = '\u252b'
boxw = 80
vline = VERTICAL_LINE
hline = HORIZONTAL_LINE
junction = INTERSECTION
ltab = 2
lines = []

def gethline(l):
  return hline*(boxw-1)+junction

def text(l):
  l = ' '*ltab+l
  return l.ljust(boxw-1)+vline
  
def center(l):
  l = ' '*ltab+l.replace('$center$','')
  return l.center(boxw-1)+vline
  
def right(l):
  l = ' '*ltab+l.replace('$center$','')
  return l.lright(boxw-1)+vline

keywords = {
  "center": center,
  "right": right,
  "line": gethline,
}

with open('resultado.txt',encoding='utf8') as f:
  for l in f:
    l = l.strip()
    tag = False
    for kw, func in keywords.items():
      if '$'+kw+'$' in l:
        l = func(l)
        tag = True
    if not tag:
      l = text(l)
    lines.append(l)
    
with open('resultado_formatado.txt','w',encoding='utf8') as f:
  f.write('\n'.join(lines))



