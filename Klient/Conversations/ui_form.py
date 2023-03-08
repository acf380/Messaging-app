# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'form.ui'
##
## Created by: Qt User Interface Compiler version 6.4.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QLabel, QPushButton, QScrollArea,
    QSizePolicy, QTabWidget, QLineEdit, QWidget, QFormLayout, QGroupBox, QVBoxLayout, QHBoxLayout)

class Ui_Widget(object):
    def setupUi(self, Widget):
        if not Widget.objectName():
            Widget.setObjectName(u"Widget")
        Widget.resize(631, 679)

        font = QFont()
        font.setPointSize(12)

        Widget.setFont(font)

        self.tabWidget = QTabWidget(Widget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tabWidget.setGeometry(QRect(10, 10, 611, 661))

        self.Conversations = QWidget()
        self.Conversations.setObjectName(u"Conversations")

        self.titleCoversations = QLabel(self.Conversations)
        self.titleCoversations.setObjectName(u"titleCoversations")
        self.titleCoversations.setGeometry(QRect(0, 0, 609, 61))
        self.titleCoversations.setMaximumSize(QSize(609, 16777215))

        font1 = QFont()
        font1.setPointSize(20)
        font1.setBold(True)

        self.titleCoversations.setFont(font1)
        self.titleCoversations.setTextFormat(Qt.PlainText)
        self.titleCoversations.setAlignment(Qt.AlignCenter)
        self.titleCoversations.setMargin(10)

        self.scrollAreaConverations = QScrollArea(self.Conversations)
        self.scrollAreaConverations.setObjectName(u"scrollAreaConverations")
        self.scrollAreaConverations.setGeometry(QRect(10, 60, 591, 561))
        self.scrollAreaConverations.setWidgetResizable(True)

        self.buttonFont = QFont()
        self.buttonFont.setPointSize(12)
        self.buttonFont.setBold(True)


        self.tabWidget.addTab(self.Conversations, "")

        self.Invite = QWidget()
        self.Invite.setObjectName(u"Invite")

        self.titleInvite = QLabel(self.Invite)
        self.titleInvite.setObjectName(u"titleInvite")
        self.titleInvite.setGeometry(QRect(10, 0, 591, 61))
        self.titleInvite.setFont(font1)
        self.titleInvite.setAlignment(Qt.AlignCenter)

        self.nickInputInvite = QLineEdit(self.Invite)
        self.nickInputInvite.setObjectName(u"nickInputInvite")
        self.nickInputInvite.setEnabled(True)
        self.nickInputInvite.setGeometry(QRect(80, 70, 281, 21))

        self.nickInvite = QLabel(self.Invite)
        self.nickInvite.setObjectName(u"nickInvite")
        self.nickInvite.setGeometry(QRect(20, 70, 49, 16))

        font3 = QFont()
        font3.setPointSize(14)
        font3.setBold(True)

        self.nickInvite.setFont(font3)

        self.idInvite = QLabel(self.Invite)
        self.idInvite.setObjectName(u"idInvite")
        self.idInvite.setGeometry(QRect(380, 70, 31, 16))
        self.idInvite.setFont(font3)

        self.idInputInvite = QLineEdit(self.Invite)
        self.idInputInvite.setObjectName(u"idInputInvite")
        self.idInputInvite.setGeometry(QRect(420, 70, 161, 21))

        self.tabWidget.addTab(self.Invite, "")

        self.Invitations = QWidget()
        self.Invitations.setObjectName(u"Invitations")

        self.titleInvitation = QLabel(self.Invitations)
        self.titleInvitation.setObjectName(u"titleInvitation")
        self.titleInvitation.setGeometry(QRect(10, 0, 591, 61))
        self.titleInvitation.setFont(font1)
        self.titleInvitation.setAlignment(Qt.AlignCenter)

        self.scrollAreaInvitation = QScrollArea(self.Invitations)
        self.scrollAreaInvitation.setObjectName(u"scrollAreaInvitation")
        self.scrollAreaInvitation.setGeometry(QRect(10, 60, 591, 561))
        self.scrollAreaInvitation.setWidgetResizable(True)

        self.tabWidget.addTab(self.Invitations, "")

        self.retranslateUi(Widget)

        self.tabWidget.setCurrentIndex(0)
        print(self.tabWidget.currentIndex())

        QMetaObject.connectSlotsByName(Widget)
    # setupUi


    def add_new_button(self, name, obj_name, scroll_area=None, x=0, y=0, w=0, h=0):
        button = QPushButton(scroll_area)
        button.setObjectName(obj_name)
        button.setFont(self.buttonFont)
        button.setGeometry(QRect(x, y, w, h))
        button.setAutoFillBackground(False)
        button.setText(QCoreApplication.translate("Widget", name, None))

        return button


    def add_person_buttons(self, names):
        formLayout = QVBoxLayout()
        formLayout.setAlignment(Qt.AlignTop)
        groupBox = QGroupBox()

        person_button_list = []
        button = None
        for name in names:
            button = self.add_new_button(name, "personButton")
            formLayout.addWidget(button)
            person_button_list.append(button)

        groupBox.setLayout(formLayout)
        self.scrollAreaConverations.setWidget(groupBox)
        self.scrollAreaConverations.setWidgetResizable(True)
        self.scrollAreaConverations.setFixedHeight(550)

        return person_button_list


    def add_dialog_buttons(self, names):
        verticalLayout = QVBoxLayout()
        verticalLayout.setAlignment(Qt.AlignTop)

        groupBox = QGroupBox()

        dialog_buttons_list = []
        buttons = [None, None, None]
        for i, name in enumerate(names):
            nick = QLabel(self.Invite)
            nick.setObjectName(f"{name}{i}")
            nick.setFont(self.buttonFont)
            nick.setText(QCoreApplication.translate("Widget", name, None))

            horizontalLayout = QHBoxLayout()
            horizontalLayout.setAlignment(Qt.AlignLeft)

            buttons = nick, self.add_new_button("Accept", f"acceptButton{i}"), self.add_new_button("Decline", f"declineButton{i}")
            dialog_buttons_list.append(buttons)

            horizontalLayout.addWidget(nick)
            horizontalLayout.addWidget(buttons[1])
            horizontalLayout.addWidget(buttons[2])
            verticalLayout.addLayout(horizontalLayout)

        groupBox.setLayout(verticalLayout)
        self.scrollAreaInvitation.setWidget(groupBox)
        self.scrollAreaInvitation.setWidgetResizable(True)
        self.scrollAreaInvitation.setFixedHeight(550)

        return dialog_buttons_list




    def retranslateUi(self, Widget):
        Widget.setWindowTitle(QCoreApplication.translate("Widget", u"The Worst GG Ever", None))
        self.titleCoversations.setText(QCoreApplication.translate("Widget", u"Conversations", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.Conversations), QCoreApplication.translate("Widget", u"Conversations", None))
        self.titleInvite.setText(QCoreApplication.translate("Widget", u"Invite to friends", None))
        self.nickInvite.setText(QCoreApplication.translate("Widget", u"Nick :", None))
        self.idInvite.setText(QCoreApplication.translate("Widget", u"ID :", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.Invite), QCoreApplication.translate("Widget", u"Invite", None))
        self.titleInvitation.setText(QCoreApplication.translate("Widget", u"Invitations", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.Invitations), QCoreApplication.translate("Widget", u"Invitations", None))


        #names = [chr(i) for i in range(65, 91)]
        #self.add_person_buttons(names)
        self.inv_but = self.add_new_button("Invite", "inviteButton", self.Invite, 420, 100, 161, 21)
        #self.add_dialog_buttons(names)


    # retranslateUi

