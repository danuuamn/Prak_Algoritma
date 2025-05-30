#include <iostream>
#include <string>
using namespace std;

struct Video {
    string judul;
    int durasi;
    string status;
    Video *left;
    Video *right;
};

struct NodeQueue {
    Video *video;
    NodeQueue *next;
};

struct NodeStack {
    Video *video;
    NodeStack *next;
};

struct UndoAction {
    string aksi;
    Video *video;
    UndoAction *next;
};

Video *root = NULL;
NodeQueue *playlistFront = NULL;
NodeQueue *playlistRear = NULL;
NodeStack *riwayatTop = NULL;
UndoAction *undoTop = NULL;

void pushRiwayat(Video *video) {
    NodeStack *baru = new NodeStack;
    baru->video = video;
    baru->next = riwayatTop;
    riwayatTop = baru;
}

Video *popRiwayat() {
    if (riwayatTop == NULL) return NULL;
    NodeStack *hapus = riwayatTop;
    Video *video = hapus->video;
    riwayatTop = riwayatTop->next;
    delete hapus;
    return video;
}

void pushUndo(string aksi, Video *video) {
    UndoAction *baru = new UndoAction;
    baru->aksi = aksi;
    baru->video = video;
    baru->next = undoTop;
    undoTop = baru;
}

UndoAction *popUndo() {
    if (undoTop == NULL) return NULL;
    UndoAction *hapus = undoTop;
    undoTop = undoTop->next;
    return hapus;
}

void enqueue(Video* video) {
    NodeQueue *baru = new NodeQueue;
    baru->video = video;
    baru->next = NULL;
    if (playlistFront == NULL) {
        playlistFront = playlistRear = baru;
    } else {
        playlistRear->next = baru;
        playlistRear = baru;
    }
}

Video *dequeue() {
    if (playlistFront == NULL) return NULL;
    NodeQueue *hapus = playlistFront;
    Video *video = hapus->video;
    playlistFront = playlistFront->next;
    if (playlistFront == NULL) playlistRear = NULL;
    delete hapus;
    return video;
}

bool tree(Video *node, string judul) {
    if (node == NULL) return false;
    if (node->judul == judul) return true;
    if (judul < node->judul)
        return tree(node->left, judul);
    else
        return tree(node->right, judul);
}

Video *insertVideo(Video *node, string judul, int durasi) {
    if (node == NULL) {
        Video *baru = new Video;
        baru->judul = judul;
        baru->durasi = durasi;
        baru->status = "tersedia";
        baru->left = baru->right = NULL;
        if (root == NULL) root = baru;
        return baru;
    }
    if (judul < node->judul)
        node->left = insertVideo(node->left, judul, durasi);
    else
        node->right = insertVideo(node->right, judul, durasi);
    return node;
}

void inOrder(Video *node) {
    if (node != NULL) {
        inOrder(node->left);
        cout << "- " << node->judul << " (" << node->durasi << " menit, " << node->status << ")" << endl;
        inOrder(node->right);
    }
}

Video *cariVideo(Video *node, string judul) {
    if (node == NULL) return NULL;
    if (node->judul == judul) return node;
    if (judul < node->judul)
        return cariVideo(node->left, judul);
    else
        return cariVideo(node->right, judul);
}

void tambahVideo() {
    string judul;
    int durasi;
    cout << "Judul: ";
    getline(cin, judul);
    if (tree(root, judul)) {
        cout << "Judul sudah ada." << endl;
        return;
    }
    cout << "Durasi (menit): ";
    cin >> durasi;
    cin.ignore();
    Video *vid = insertVideo(root, judul, durasi);
    pushUndo("tambah", vid);
    cout << "Video berhasil ditambahkan." << endl;
}

void tampilkanDaftar() {
    cout << "Daftar Video:" << endl;
    inOrder(root);
    cout << "Cari video (y/t)? ";
    char cari;
    cin >> cari;
    cin.ignore();
    if (cari == 'y') {
        string judul;
        cout << "Judul yang dicari: ";
        getline(cin, judul);
        Video *vid = cariVideo(root, judul);
        if (vid != NULL)
            cout << "Ditemukan: " << vid->judul << ", " << vid->durasi << " menit" << endl;
        else
            cout << "Tidak ditemukan." << endl;
    }
}

void tambahKePlaylist() {
    string judul;
    cout << "Judul video yang ingin dimasukkan ke playlist: ";
    getline(cin, judul);
    Video* vid = cariVideo(root, judul);
    if (vid == NULL) {
        cout << "Video tidak ditemukan." << endl;
        return;
    }
    if (vid->status != "tersedia") {
        cout << "Video tidak tersedia untuk dimasukkan ke playlist." << endl;
        return;
    }
    enqueue(vid);
    vid->status = (playlistFront->video == vid) ? "sedang diputar" : "dalam antrean";
    pushUndo("playlist", vid);
    cout << "Video berhasil masuk playlist." << endl;
}

void tontonVideo() {
    Video *vid = dequeue();
    if (vid == NULL) {
        cout << "Playlist kosong." << endl;
        return;
    }
    cout << "Memutar: " << vid->judul << endl;
    vid->status = "tersedia";
    pushRiwayat(vid);
    pushUndo("tonton", vid);
    if (playlistFront != NULL) playlistFront->video->status = "sedang diputar";
}

void tampilkanRiwayat() {
    cout << "Riwayat Tontonan: " << endl;
    NodeStack *temp = riwayatTop;
    while (temp != NULL) {
        cout << "- " << temp->video->judul << endl;
        temp = temp->next;
    }
}

void undoTerakhir() {
    UndoAction *last = popUndo();
    if (last == NULL) {
        cout << "Tidak ada aksi yang bisa di-undo." << endl;
        return;
    }
    if (last->aksi == "tambah") {
        cout << "Undo Tambah: fitur hapus dari pohon belum tersedia." << endl;
    } else if (last->aksi == "playlist") {
        cout << "Undo Playlist: belum diimplementasi hapus dari queue." << endl;
    } else if (last->aksi == "tonton") {
        Video *v = popRiwayat();
        enqueue(v);
        v->status = "sedang diputar";
        cout << "Undo Tonton: " << v->judul << " dikembalikan ke playlist." << endl;
    }
    delete last;
}

int main() {
    system("cls");
    int pilihan;
    char ulang;
    do {
        cout << "--- IDLIX Tube ---" << endl;
        cout << "1. Tambah Video" << endl;
        cout << "2. Tampilkan Daftar Video" << endl;
        cout << "3. Tambahkan ke Playlist" << endl;
        cout << "4. Tonton Video" << endl;
        cout << "5. Riwayat Tontonan" << endl;
        cout << "6. Undo Tindakan Terakhir" << endl;
        cout << "0. Keluar" << endl;
        cout << "Pilihan: ";
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1: tambahVideo(); break;
            case 2: tampilkanDaftar(); break;
            case 3: tambahKePlaylist(); break;
            case 4: tontonVideo(); break;
            case 5: tampilkanRiwayat(); break;
            case 6: undoTerakhir(); break;
            case 0: cout << "Keluar"; exit(0); break;
            default: cout << "Pilihan tidak valid." << endl; break;
        }
        cout << endl;
        cout << "Kembali ke Menu? (Y/N) : "; cin >> ulang;
    } while (ulang == 'y' || ulang == 'Y');
    cout << "Terima kasih telah menggunakan IDLIX Tube!" << endl;
    return 0;
}
