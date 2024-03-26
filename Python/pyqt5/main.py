# coding：utf-8

import sys
from PyQt5.QtWidgets import QMainWindow, QAction, qApp, QMenu, QApplication, QWidget,QLabel,QHBoxLayout,QVBoxLayout,QPushButton
from PyQt5.QtGui import QIcon, QContextMenuEvent


class Frame(QMainWindow):

    def __init__(self):
        super().__init__()

        # 初始化状态栏界面
        self.initStatus()

        # 初始化工具栏
        self.initTool()


        # 设置窗口初始化位置和大小，窗口名称，显示窗口
        self.setGeometry(300, 300, 800, 500)
        self.setWindowTitle('Statusbar')
        self.show()

    def initTool(self):
        '''初始化菜单界面工具栏'''
        exitAct = QAction(QIcon(''),'&Exit',self)
        exitAct.setStatusTip('退出消息发送工具')
        exitAct.triggered.connect(qApp.quit)

        self.toolbar = self.addToolBar('Exit')
        self.toolbar.addAction(exitAct)

    def initStatus(self):
        '''初始化菜单界面状态栏 '''
        # 创建一个状态栏
        self.statusBar()

        # 创建一个菜单栏
        menubar = self.menuBar()
        # 在菜单栏添加一个file标签
        fileMenu = menubar.addMenu('&File')

        # 在标签中添加退出按钮，设置按钮图片 名称 快捷键 备注信息 功能
        exitAct = QAction(QIcon(''),'&Exit',self)
        exitAct.setStatusTip('退出消息发送工具')
        exitAct.triggered.connect(qApp.quit)
        fileMenu.addAction(exitAct)
        
        # 添加打开文件功能
        self.initOpenFileMenu(fileMenu)
        # 添加自动保存功能
        self.initSave(fileMenu)

    def initOpenFileMenu(self,parentMenu:QMenu):
        '''在指定的菜单目录下，添加打开文件目录，返回新建的菜单目录'''
        # 打开指定csv文件
        openCsv = QAction('csv file',self)
        openCsv.setStatusTip('打开已被解析后的csv文件')

        # 打开指定ue4 log文件
        openLog = QAction('log file',self)
        openLog.setStatusTip('打开ue4 log文件')

        # 创建菜单，添加按钮
        openMenu = QMenu('open',self)
        openMenu.setStatusTip('打开文件')
        openMenu.setShortcutAutoRepeat
        openMenu.addAction(openCsv)
        openMenu.addAction(openLog)
        parentMenu.addMenu(openMenu)

    def initSave(self,parentMenu:QMenu):
        '''在指定的菜单目录下，添加保存与自动保存选项，返回新建的菜单目录'''
        # 保存文件
        saveThis = QAction('save',self)
        saveThis.setStatusTip('保存当前文件')
        saveThis.setShortcut('Ctrl+S')
        # 另存为
        saveAs = QAction('save as',self)
        saveAs.setStatusTip('另存为当前文件')
        # 自动保存功能
        AutoSave = QAction('auto save',self,checkable=True)
        AutoSave.setStatusTip('开启自动保存功能')
        AutoSave.setChecked(True)

        parentMenu.addAction(saveThis)
        parentMenu.addAction(saveAs)
        parentMenu.addAction(AutoSave)
        
    def contextMenuEvent(self, event: QContextMenuEvent) -> None:
        '''处理右键弹窗事件'''
        cmenu = QMenu(self)
        newAct1 = cmenu.addAction('null button1')
        saveThis = cmenu.addAction('save')
        saveThis.setStatusTip('保存当前文件')
        quitAct = cmenu.addAction('Quit')
        action = cmenu.exec_(self.mapToGlobal(event.pos()))
        if action == quitAct:
            qApp.quit()

        return super().contextMenuEvent(event)


if __name__ == '__main__':
    # 初始化一个App
    app = QApplication(sys.argv)
    # 创建一个窗口
    ex = Frame()

    # 等待窗口关闭并结束运行
    sys.exit(app.exec_())
