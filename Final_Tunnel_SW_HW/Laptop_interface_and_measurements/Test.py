import pyqtgraph as pg
from PyQt5 import QtWidgets
import sys

app = QtWidgets.QApplication([])
win = pg.GraphicsLayoutWidget(title="Test Plot")
plot = win.addPlot(title="Hello Plot")
plot.plot([1, 2, 3], [4, 2, 5])
win.show()
sys.exit(app.exec_())