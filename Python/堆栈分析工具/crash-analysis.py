import re
import sys
from PyQt5.QtWidgets import QApplication,QMainWindow
from PyQt5 import QtGui
import hello
def crash():
    symbolPath = r".\symbol.txt"
    crashPath = r".\NIC.log"
    pattern = "\]LogAndroid: Error: \[Callstack\] 0x.*? libUE4.so\(0x(.*?)\)!\[Unknown\]\(\)  \[\]"
    stacks = ""
    crashStack = []
    with open(crashPath,"r",errors="ignore") as f:
        lines = f.readlines()
        for i in lines:
            res = re.search(pattern,i)
            if res:
                crashStack.append(res.group(1))
    with open(symbolPath,'r',errors='ignore') as f:
        lines = f.readlines()
        for cs in crashStack:
            for i in lines:
                strs = re.split(r"[ \t]+", i)
                if(isinstance(strs,list) and len(strs) > 5):
                    low = int(strs[0],16)
                    up = int(strs[0],16) + int(strs[4],16)
                    ics = int(cs,16)
                    if(low < ics < up):
                        stacks += i
                        break
    return stacks
class UIWindows():

    def __init__(self):
        self.MainWindow = QMainWindow()
        # self.MainWindow.setWindowFlags(self.MainWindow.windowFlags()|QtCore.Qt.WindowStaysOnTopHint)
        self.ui = hello.Ui_MainWindow()
        self.ui.setupUi(self.MainWindow)
        self.ui.pushButton.clicked.connect(self.click_send)
        self.MainWindow.show()
    def click_send(self):
        res = crash()
        print(res)
        self.ui.textEdit.setWordWrapMode(QtGui.QTextOption.NoWrap)
        self.ui.textEdit.setText(res)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    windows = UIWindows()
    sys.exit(app.exec_())