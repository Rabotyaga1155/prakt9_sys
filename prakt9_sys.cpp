#include <windows.h>
#include <cmath>
#include <iostream>

const int max = 4;
struct City {
    int x, y;
} cities[max];

double distance(const City& a, const City& b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

int tour[max]; //Хранит порядок
bool used[max];//Отслеживает посещенные города
int best = -1; // Индекс ближайшего города
CRITICAL_SECTION cs;

DWORD WINAPI findBestCity(LPVOID lpParam) {
    int j = *(int*)lpParam;
    int i = tour[0];
    if (!used[j] && (best == -1 || distance(cities[i - 1], cities[j]) < distance(cities[i - 1], cities[best]))) {
        EnterCriticalSection(&cs);
        best = j;
        LeaveCriticalSection(&cs);
    }
    return 0;
    
}
void findRoad(int n) {
    std::fill(used, used + n, false);
    tour[0] = 0;
    used[0] = true;
    InitializeCriticalSection(&cs);

    for (int i = 1; i < n; ++i) {
        best = -1;
        HANDLE threads[max];
        int indices[max];

        for (int j = 0; j < n; ++j) {
            indices[j] = j;
            threads[j] = CreateThread(NULL, 0, findBestCity, &indices[j], 0, NULL);
        }

        WaitForMultipleObjects(n, threads, TRUE, INFINITE);

        for (int j = 0; j < n; ++j) {
            CloseHandle(threads[j]);
        }

        tour[i] = best;
        used[best] = true;
    }

    DeleteCriticalSection(&cs);
}

int main() {
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::cin >> cities[i].x >> cities[i].y;
    }

    findRoad(n);

    for (int i = 0; i < n; ++i) {
        std::cout << tour[i] << ' ';
    }
    std::cout << std::endl;

    return 0;
}
