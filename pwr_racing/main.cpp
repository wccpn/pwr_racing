#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <tuple>
#include <iostream>
#include <stack>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
// program był kompilowany komendą clang++ main.cpp -lfmt
// do obliczania otoczki wypukłej został użyty algorytm Grahama (złożoność obliczeniowa n*log(n)) https://pl.wikipedia.org/wiki/Algorytm_Grahama 
// do formatowania wyjścia w pożadany sposób użyta została biblioteka {fmt} https://github.com/fmtlib/fmt
// plik z dodatkowym wytłumaczeniem uznałem za nie potrzebny, ponieważ powtarzałbym po prostu komentarze

// struct point do przechowywania danych o punktach
struct Point {
  double x, y;
};
// funkcja do obliczania dystansu między punktami (potrzebna w przypadku gdy punkty są położone współliniowo)
double distance_sqared(const Point& p1, const Point& p2){
  return std::pow(p1.x - p2.x, 2)  + std::pow(p1.y - p2.y, 2);
}
// funkcja do określania orientacji 3 punktów względem siebie 
int orientation(const Point& p, const Point& q, const Point& r) {
  const double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  if (static_cast<int>(val) == 0) {
    return 0; // punkty są współliniowe
  }
  else if (val > 0) {
    return 1; // orientacja względem wskazówek zegara
  }
  else {
    return -1; // orientacja przeciwnie do ruchu wskazówek zegara
  }
}

Point p0;
bool compare(const Point& p1, const Point& p2) {
  int o = orientation(p0, p1, p2);
  if (o == 0) {
    // w przpadku gdy funkcje są współliniowe sprawdzamy dystanse między punktami
    return distance_sqared(p0, p1) < distance_sqared(p0, p2);
  }
  // sprawdzamy czy orientacja jest przeciwna do ruchu wskazówek zegara  
  return o == -1;
}

// algorytm Grahama
std::vector<Point> convexHull(std::vector<Point>& points, int n) {
  // dla mniej niż 3 punktów nie możliwe jest stworzenie otoczki
  if (n < 3) {
    return {};
  }
  // szukamy punktu o najmniejszej wartości y (jeśli jest wiele takich samych, to szukamy tego o najmniejszej wartości x)
  int min = 0;
  for (int i = 1; i < n; i++) {
    if ((points[i].y < points[min].y) || (points[i].y == points[min].y && points[i].x < points[min].x))
            min = i;
  }
  std::swap(points[0], points[min]);
  p0 = points[0];
  // sortujemy punkty według ich orientacji
  sort(points.begin() + 1, points.end(), compare);

  // stos do przechowywania punktów
  std::stack<Point> hull;
  hull.push(points[0]);
  hull.push(points[1]);
  hull.push(points[2]);
  // sprawdzamy pozostałe punkty
   for (int i = 3; i < n; i++) {
     while (hull.size() > 1) {
       Point second = hull.top();
       hull.pop();
       Point first = hull.top();
       if (orientation(first, second, points[i]) == -1) {
         hull.push(second);
         break;
       }
     }
     hull.push(points[i]);
   }
   // przenosimy wyniki z stosu na vector
   std::vector<Point> hullPoints;
   while (!hull.empty()) {
     hullPoints.push_back(hull.top());
     hull.pop();
   }
   return hullPoints;
}

int main(int argc, char* argv[]) {
  std::ifstream file(argv[1]);
  if (!file) {
    std::cerr << "Nie można otworzyć pliku\n";
    return 1;
  }
  // wprowadzamy do zmiennych dane z pliku
  int n;
  file >> n;
  std::vector<Point> points(n);
  for (int i = 0; i < n; i++) {
    file >> points[i].x >> points[i].y;
  }
  // używamy algorytmu Grahama
  const std::vector<Point> hull = convexHull(points, n);
  // zamieniamy wynik na pożądany format 
  std::vector<std::tuple<double, double>> result;
  for (const auto& point : hull) {
    result.push_back(std::make_tuple(point.x, point.y));
  }
  // niestety trzeba było tutaj użyć zewnętrznej bilblioteki do formatowania, bo przy użyciu funkcji std::println() program nie chciał się kompilować
  fmt::println("{}", result);
  
  return 0;
}
