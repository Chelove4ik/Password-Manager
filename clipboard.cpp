#include "clipboard.h"

bool setClipboard(const std::string &line) {
    if (!OpenClipboard(NULL))
        return FALSE;
    EmptyClipboard();

    HGLOBAL hglbCopy = GlobalAlloc(GMEM_FIXED, (1 + line.size()) * sizeof(char));
    if (hglbCopy == nullptr) {
        CloseClipboard();
        return false;
    }
    memcpy(hglbCopy, line.c_str(), (1 + line.size()) * sizeof(char));
    SetClipboardData(CF_TEXT, hglbCopy);

    CloseClipboard();
    return true;
}