#coding：utf-8
from fontTools.ttLib import TTFont
import os

fontType = os.path.join("fonts", r"C:\Users\Administrator\Downloads\dovemayo\Dovemayo_gothic.ttf")

font = TTFont(fontType)
uniMap = font['cmap'].tables[0].ttFont.getBestCmap()

print ('英语', ord('a') in uniMap.keys())
print ('数字', ord('5') in uniMap.keys())
print ('中文', ord('好') in uniMap.keys())
print ('日语', ord('よ') in uniMap.keys())
print ('韩语', ord('좋') in uniMap.keys())
print ('泰语', ord('ด') in uniMap.keys())
print ('俄语', ord('д') in uniMap.keys())