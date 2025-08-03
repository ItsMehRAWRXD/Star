import sys
import os
from PyQt5 import QtWidgets, QtCore
import secrets

# --- Key handling (shared with C++ code) ---
XOR_OBFUSCATE_KEY = 0x5A
ENC_KEY = bytes([0x39, 0x39, 0x08, 0x0F, 0x0F, 0x38, 0x08, 0x31, 0x38, 0x32, 0x38])

# Try to get key from environment variable first
def get_deobfuscated_key():
    env_key = os.getenv("ENCRYPTION_KEY")
    if env_key and len(env_key) >= len(ENC_KEY):
        return env_key[:len(ENC_KEY)].encode('utf-8')
    else:
        # Fallback to obfuscated key
        return bytes([b ^ XOR_OBFUSCATE_KEY for b in ENC_KEY])

KEY_DEOBFUSCATED = get_deobfuscated_key()

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


def aes_ctr_crypt(data: bytes, key: bytes, nonce: bytes = None) -> tuple[bytes, bytes]:
    if AES is None:
        raise RuntimeError("PyCryptodome is required for AES operations. Install with: pip install pycryptodome")
    # Expand/truncate key to 16 bytes for AES-128
    key16 = (key * (16 // len(key) + 1))[:16]
    
    # Generate nonce if not provided
    if nonce is None:
        nonce = secrets.token_bytes(16)
    
    cipher = AES.new(key16, AES.MODE_CTR, nonce=nonce)
    encrypted_data = cipher.encrypt(data)  # encryption and decryption are symmetric in CTR
    return encrypted_data, nonce


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
                out_path = self._get_output_path(filepath, ".xor")
            else:  # AES
                if self.decryptCheck.isChecked():
                    # For decryption, read nonce from file
                    if len(data) < 16:
                        raise ValueError("File too small to contain nonce")
                    nonce = data[:16]
                    encrypted_data = data[16:]
                    processed, _ = aes_ctr_crypt(encrypted_data, key, nonce)
                else:
                    # For encryption, generate new nonce
                    processed, nonce = aes_ctr_crypt(data, key)
                    # Prepend nonce to encrypted data
                    processed = nonce + processed
                
                out_path = self._get_output_path(filepath, ".aes")

            with open(out_path, "wb") as f:
                f.write(processed)

            QtWidgets.QMessageBox.information(self, "Success", f"Output written to: {out_path}")
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Error", str(e))
    
    def _get_output_path(self, filepath: str, suffix: str) -> str:
        base, ext = os.path.splitext(filepath)
        if self.decryptCheck.isChecked():
            return base + ".dec" + ext
        else:
            return base + ".enc" + ext


def main():
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()