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
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QVBoxLayout, QWidget)

class Ui_Widget(object):
    def setupUi(self, Widget):
        if not Widget.objectName():
            Widget.setObjectName(u"Widget")
        Widget.resize(552, 657)
        self.gridLayout = QGridLayout(Widget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.WholeScreenVerticalLayout = QVBoxLayout()
        self.WholeScreenVerticalLayout.setObjectName(u"WholeScreenVerticalLayout")
        self.verticalSpacer_2 = QSpacerItem(20, 280, QSizePolicy.Minimum, QSizePolicy.Preferred)

        self.WholeScreenVerticalLayout.addItem(self.verticalSpacer_2)

        self.LoginAndPasswordHorizontalLayout = QHBoxLayout()
        self.LoginAndPasswordHorizontalLayout.setObjectName(u"LoginAndPasswordHorizontalLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.LoginAndPasswordHorizontalLayout.addItem(self.horizontalSpacer)

        self.LogiLabel = QLabel(Widget)
        self.LogiLabel.setObjectName(u"LogiLabel")

        self.LoginAndPasswordHorizontalLayout.addWidget(self.LogiLabel)

        self.LoginLineEdit = QLineEdit(Widget)
        self.LoginLineEdit.setObjectName(u"LoginLineEdit")

        self.LoginAndPasswordHorizontalLayout.addWidget(self.LoginLineEdit)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.LoginAndPasswordHorizontalLayout.addItem(self.horizontalSpacer_2)

        self.PassworLabel = QLabel(Widget)
        self.PassworLabel.setObjectName(u"PassworLabel")

        self.LoginAndPasswordHorizontalLayout.addWidget(self.PassworLabel)

        self.PasswordLineEdit = QLineEdit(Widget)
        self.PasswordLineEdit.setObjectName(u"PasswordLineEdit")

        self.LoginAndPasswordHorizontalLayout.addWidget(self.PasswordLineEdit)

        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.LoginAndPasswordHorizontalLayout.addItem(self.horizontalSpacer_3)


        self.WholeScreenVerticalLayout.addLayout(self.LoginAndPasswordHorizontalLayout)

        self.verticalSpacer_3 = QSpacerItem(20, 15, QSizePolicy.Minimum, QSizePolicy.Maximum)

        self.WholeScreenVerticalLayout.addItem(self.verticalSpacer_3)

        self.LogInhorizontalLayout = QHBoxLayout()
        self.LogInhorizontalLayout.setObjectName(u"LogInhorizontalLayout")
        self.horizontalSpacer_4 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.LogInhorizontalLayout.addItem(self.horizontalSpacer_4)

        self.LogInButton = QPushButton(Widget)
        self.LogInButton.setObjectName(u"LogInButton")
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.LogInButton.sizePolicy().hasHeightForWidth())
        self.LogInButton.setSizePolicy(sizePolicy)
        self.LogInButton.setBaseSize(QSize(0, 0))
        self.LogInButton.setIconSize(QSize(20, 20))

        self.LogInhorizontalLayout.addWidget(self.LogInButton)

        self.horizontalSpacer_5 = QSpacerItem(20, 20, QSizePolicy.Maximum, QSizePolicy.Minimum)

        self.LogInhorizontalLayout.addItem(self.horizontalSpacer_5)

        self.label = QLabel(Widget)
        self.label.setObjectName(u"label")

        self.LogInhorizontalLayout.addWidget(self.label)

        self.horizontalSpacer_6 = QSpacerItem(20, 20, QSizePolicy.Maximum, QSizePolicy.Minimum)

        self.LogInhorizontalLayout.addItem(self.horizontalSpacer_6)

        self.pushButton = QPushButton(Widget)
        self.pushButton.setObjectName(u"pushButton")

        self.LogInhorizontalLayout.addWidget(self.pushButton)

        self.horizontalSpacer_7 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.LogInhorizontalLayout.addItem(self.horizontalSpacer_7)


        self.WholeScreenVerticalLayout.addLayout(self.LogInhorizontalLayout)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Preferred)

        self.WholeScreenVerticalLayout.addItem(self.verticalSpacer)

        self.ErrorMessageLabel = QLabel(Widget)
        self.ErrorMessageLabel.setObjectName(u"")
        self.ErrorMessageLabel.setAlignment(Qt.AlignCenter)

        self.WholeScreenVerticalLayout.addWidget(self.ErrorMessageLabel)

        self.verticalSpacer_4 = QSpacerItem(20, 200, QSizePolicy.Minimum, QSizePolicy.Preferred)

        self.WholeScreenVerticalLayout.addItem(self.verticalSpacer_4)


        self.gridLayout.addLayout(self.WholeScreenVerticalLayout, 0, 0, 1, 1)


        self.retranslateUi(Widget)

        QMetaObject.connectSlotsByName(Widget)
    # setupUi

    def retranslateUi(self, Widget):
        Widget.setWindowTitle(QCoreApplication.translate("Widget", u"Widget", None))
        self.LogiLabel.setText(QCoreApplication.translate("Widget", u"Login", None))
        self.PassworLabel.setText(QCoreApplication.translate("Widget", u"Password", None))
        self.LogInButton.setText(QCoreApplication.translate("Widget", u"Log in", None))
        self.label.setText(QCoreApplication.translate("Widget", u"Or", None))
        self.pushButton.setText(QCoreApplication.translate("Widget", u"Sign up", None))
        self.ErrorMessageLabel.setText(QCoreApplication.translate("Widget", u"", None))
    # retranslateUi

