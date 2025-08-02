import sys
import os
from PyQt5 import QtWidgets, QtCore

# --- Key handling (shared with C++ code) ---
XOR_OBFUSCATE_KEY = 0x5A
ENC_KEY = bytes([0x39, 0x39, 0x08, 0x0F, 0x0F, 0x38, 0x08, 0x31, 0x38, 0x32, 0x38])
KEY_DEOBFUSCATED = bytes([b ^ XOR_OBFUSCATE_KEY for b in ENC_KEY])

try:
    from Crypto.Cipher import AES  # pycryptodome
except ImportError:
    try:
        from Cryptodome.Cipher import AES  # pycryptodomex
    except ImportError:
        AES = None

BUFFER_SIZE = 4096


def xor_crypt(data: bytes, key: bytes) -> bytes:
    klen = len(key)
    return bytes([b ^ key[i % klen] for i, b in enumerate(data)])


def aes_ctr_crypt(data: bytes, key: bytes) -> bytes:
    if AES is None:
        raise RuntimeError("PyCryptodome is required for AES operations. Install with: pip install pycryptodome")
    # Expand/truncate key to 16 bytes for AES-128
    key16 = (key * (16 // len(key) + 1))[:16]
    cipher = AES.new(key16, AES.MODE_CTR, nonce=b"")
    return cipher.encrypt(data)  # encryption and decryption are symmetric in CTR


class DropLabel(QtWidgets.QLabel):
    fileDropped = QtCore.pyqtSignal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setText("Drag and drop file here")
        self.setAlignment(QtCore.Qt.AlignCenter)
        self.setStyleSheet("QLabel{border: 2px dashed #aaa; font-size: 16px; padding: 40px;}")
        self.setAcceptDrops(True)

    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()

    def dropEvent(self, event):
        urls = event.mimeData().urls()
        if urls:
            self.fileDropped.emit(urls[0].toLocalFile())


class MainWindow(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("File Obfuscator")
        self.resize(400, 300)

        self.dropArea = DropLabel()

        # Obfuscation selection
        self.methodCombo = QtWidgets.QComboBox()
        self.methodCombo.addItems(["XOR", "AES-128-CTR"])

        # Mode selection
        self.decryptCheck = QtWidgets.QCheckBox("Decrypt Mode (encrypt unchecked)")

        layout = QtWidgets.QVBoxLayout(self)
        layout.addWidget(self.dropArea)
        layout.addWidget(QtWidgets.QLabel("Obfuscation Method:"))
        layout.addWidget(self.methodCombo)
        layout.addWidget(self.decryptCheck)
        layout.addStretch()

        self.dropArea.fileDropped.connect(self.process_file)

    def process_file(self, filepath: str):
        if not os.path.isfile(filepath):
            QtWidgets.QMessageBox.warning(self, "Error", "Dropped item is not a file.")
            return
        try:
            with open(filepath, "rb") as f:
                data = f.read()

            method = self.methodCombo.currentText()
            key = KEY_DEOBFUSCATED
            if method == "XOR":
                processed = xor_crypt(data, key)
            else:  # AES
                processed = aes_ctr_crypt(data, key)

            # Determine output filename
            base, ext = os.path.splitext(filepath)
            if self.decryptCheck.isChecked():
                out_path = base + ".dec" + ext
            else:
                out_path = base + ".enc" + ext

            with open(out_path, "wb") as f:
                f.write(processed)

            QtWidgets.QMessageBox.information(self, "Success", f"Output written to: {out_path}")
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Error", str(e))


def main():
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()