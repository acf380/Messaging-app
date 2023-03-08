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
from PySide6.QtWidgets import (QApplication, QGridLayout, QHBoxLayout, QLabel,
    QLineEdit, QPushButton, QScrollArea, QSizePolicy,
    QSpacerItem, QVBoxLayout, QWidget)

class Ui_Widget(object):
    def setupUi(self, Widget):
        if not Widget.objectName():
            Widget.setObjectName(u"Widget")
        Widget.resize(800, 600)
        self.gridLayout = QGridLayout(Widget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.BackButtonHorizontalLayout = QHBoxLayout()
        self.BackButtonHorizontalLayout.setObjectName(u"BackButtonHorizontalLayout")
        self.BackPushButton = QPushButton(Widget)
        self.BackPushButton.setObjectName(u"BackPushButton")
        font = QFont()
        font.setBold(True)
        self.BackPushButton.setFont(font)

        self.BackButtonHorizontalLayout.addWidget(self.BackPushButton)

        self.BackPushButtonHorizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.BackButtonHorizontalLayout.addItem(self.BackPushButtonHorizontalSpacer)

        self.pushButton = QPushButton(Widget)
        self.pushButton.setObjectName(u"pushButton")
        palette = QPalette()
        brush = QBrush(QColor(255, 0, 4, 255))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Text, brush)
        brush1 = QBrush(QColor(255, 0, 0, 255))
        brush1.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.ButtonText, brush1)
        brush2 = QBrush(QColor(0, 0, 0, 255))
        brush2.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Inactive, QPalette.Text, brush2)
        palette.setBrush(QPalette.Inactive, QPalette.ButtonText, brush2)
        brush3 = QBrush(QColor(120, 120, 120, 255))
        brush3.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Disabled, QPalette.Text, brush3)
        palette.setBrush(QPalette.Disabled, QPalette.ButtonText, brush3)
        self.pushButton.setPalette(palette)
        self.pushButton.setFont(font)

        self.BackButtonHorizontalLayout.addWidget(self.pushButton)


        self.verticalLayout.addLayout(self.BackButtonHorizontalLayout)

        self.MessagesScrollArea = QScrollArea(Widget)
        self.MessagesScrollArea.setObjectName(u"MessagesScrollArea")
        self.MessagesScrollArea.setWidgetResizable(True)
        self.MessagesScrollAreaWidgetContents = QWidget()
        self.MessagesScrollAreaWidgetContents.setObjectName(u"MessagesScrollAreaWidgetContents")
        self.MessagesScrollAreaWidgetContents.setGeometry(QRect(0, 0, 778, 514))
        self.gridLayout_2 = QGridLayout(self.MessagesScrollAreaWidgetContents)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.MessagesVerticalLayout = QVBoxLayout()
        self.MessagesVerticalLayout.setObjectName(u"MessagesVerticalLayout")
        self.UpperVerticalSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.MessagesVerticalLayout.addItem(self.UpperVerticalSpacer)

        self.VerticalSpacerBetweenMessages = QSpacerItem(20, 10, QSizePolicy.Minimum, QSizePolicy.Maximum)

        self.MessagesVerticalLayout.addItem(self.VerticalSpacerBetweenMessages)

        self.gridLayout_2.addLayout(self.MessagesVerticalLayout, 0, 0, 1, 1)

        self.MessagesScrollArea.setWidget(self.MessagesScrollAreaWidgetContents)

        self.verticalLayout.addWidget(self.MessagesScrollArea)

        self.SendingMessageHorizontalLayout = QHBoxLayout()
        self.SendingMessageHorizontalLayout.setObjectName(u"SendingMessageHorizontalLayout")
        self.SendingMessageLineEdit = QLineEdit(Widget)
        self.SendingMessageLineEdit.setObjectName(u"SendingMessageLineEdit")

        self.SendingMessageHorizontalLayout.addWidget(self.SendingMessageLineEdit)

        self.SendButton = QPushButton(Widget)
        self.SendButton.setObjectName(u"SendButton")
        self.SendButton.setFont(font)

        self.SendingMessageHorizontalLayout.addWidget(self.SendButton)


        self.verticalLayout.addLayout(self.SendingMessageHorizontalLayout)


        self.gridLayout.addLayout(self.verticalLayout, 0, 0, 1, 1)


        self.retranslateUi(Widget)

        QMetaObject.connectSlotsByName(Widget)
    # setupUi

    def add_receiver_label(self, text):
        MessageFromRecieverLabelHorizontalLayout = QHBoxLayout()
        MessageFromRecieverLabelHorizontalLayout.setObjectName(u"MessageFromRecieverLabelHorizontalLayout")
        MessageFromRecieverLabel = QLabel(self.MessagesScrollAreaWidgetContents)
        MessageFromRecieverLabel.setObjectName(u"MessageFromRecieverLabel")
        
        MessageFromRecieverLabelHorizontalLayout.addWidget(MessageFromRecieverLabel)
        MessageFromRecieverLabelHorizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        MessageFromRecieverLabelHorizontalLayout.addItem(MessageFromRecieverLabelHorizontalSpacer)
        self.MessagesVerticalLayout.addLayout(MessageFromRecieverLabelHorizontalLayout)
        MessageFromRecieverLabel.setText(QCoreApplication.translate("Widget", text, None))
        return MessageFromRecieverLabel


    def add_sender_label(self, text):
        MessageFromSenderLabelHorizontalLayout = QHBoxLayout()
        MessageFromSenderLabelHorizontalLayout.setObjectName(u"MessageFromSenderLabelHorizontalLayout")
        MessageFromSenderLabelHorizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)
        MessageFromSenderLabelHorizontalLayout.addItem(MessageFromSenderLabelHorizontalSpacer)
        MessageFromSenderLabel = QLabel(self.MessagesScrollAreaWidgetContents)
        MessageFromSenderLabel.setObjectName(u"MessageFromSenderLabel")
        MessageFromSenderLabelHorizontalLayout.addWidget(MessageFromSenderLabel)
        self.MessagesVerticalLayout.addLayout(MessageFromSenderLabelHorizontalLayout)
        MessageFromSenderLabel.setText(QCoreApplication.translate("Widget", text, None))
        
        return MessageFromSenderLabel


    def retranslateUi(self, Widget):
        Widget.setWindowTitle(QCoreApplication.translate("Widget", u"Widget", None))
        self.BackPushButton.setText(QCoreApplication.translate("Widget", u"Back", None))
        #self.pushButton.setText(QCoreApplication.translate("Widget", u"Delete", None))
        self.SendButton.setText(QCoreApplication.translate("Widget", u"Send", None))

