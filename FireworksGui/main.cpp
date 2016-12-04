/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGui.

    FireworksGui is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGui is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGui.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#ifdef Q_OS_ANDROID
    #include "mainandroidwindow.h"
#else
    #include "mainwindow.h"
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef Q_OS_ANDROID
    MainAndroidWindow w;
#else
    MainWindow w;
#endif
    w.show();

    return a.exec();
}
