import re, os

VERTICAL_LINE = '\u2503'
HORIZONTAL_LINE = '\u2501'
INTERSECTION = '\u252b'
INTERSECTION_INV = '\u2523'
UPPERLINE = '\u23ba'
UPPER_CORNER = '\u251b'
LOWER_CORNER = '\u2513'
UPPER_CORNER_INV = '\u2517'
LOWER_CORNER_INV = '\u250f'

boxw = 80
vline = VERTICAL_LINE
hline = HORIZONTAL_LINE
junction = INTERSECTION
junctioni = INTERSECTION_INV
ucorner = UPPER_CORNER
ucorneri = UPPER_CORNER_INV
lcorner = LOWER_CORNER
lcorneri = LOWER_CORNER_INV
special_chars = [vline,hline,junction,junctioni,ucorner,ucorneri,lcorner,lcorneri]
ltab = 2
rtab = 2
lines = []

def getfreespace():
  return boxw-ltab-rtab-1-1

def gethline(l): #l is not used
  return junctioni+hline*(boxw-2)+junction

def getstartingline():
  return lcorneri+hline*(boxw-2)+lcorner

def getendline():
  return ucorneri+hline*(boxw-2)+ucorner

def text(l):
  l = ' '*ltab+l+' '*rtab
  return vline+l.ljust(boxw-2)+vline

def right(l):
  l = ' '*ltab+l+' '*rtab
  return vline+l.rjust(boxw-2)+vline

def center(l):
  return vline+l.center(boxw-2)+vline

def underline(l):
  beg = 0
  end = len(l)-1
  i = 0
  found = False
  for c in l:
    if not c in [' ']+special_chars:
      if beg == 0:
        beg = i
      end = i
      found = True
    i += 1
  if not found:
    return l
  new_l = l[:beg] + UPPERLINE*(end-beg+1) + l[end+1:]
  return l+'\n'+new_l

keywords = {
  "center": center,
  "right": right,
  "line": gethline,
  "underline": underline
}

filename, ext = os.path.splitext(input("Input (sem extensão): "))
i = 0
with open(filename+ext,encoding='utf8') as f:
  for l in f:
    filtres = []
    l = l.strip()
    tag = False
    for kw, func in keywords.items():
      if '$'+kw+'$' in l:
        l = l.replace('$'+kw+'$','')
        filtres.append(func)
        tag = True
    if( len(l) > getfreespace() ):
      print("[!] Warning! Line %d truncated!" % (i))
    l = l.strip()
    l = l[:getfreespace()]
    for func in filtres:
      l = func(l)
    if not tag:
      l = text(l.replace('@',''))
    lines.append(l)
    i += 1

with open(filename+'_formatado'+ext,'w',encoding='utf8') as f:
  f.write(getstartingline()+'\n'+'\n'.join(lines)+'\n'+getendline())

print("Saída em "+filename+'_formatado'+ext)
