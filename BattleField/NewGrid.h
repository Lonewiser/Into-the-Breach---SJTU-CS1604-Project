#ifndef NEWGRID_H_INCLUDED
#define NEWGRID_H_INCLUDED

#include <vector>
#include <cassert>
#include <iostream>
#include <iterator>

template <typename T>
class Grid {
private:
    std::vector<std::vector<T>> data;

    class ProxyRow {
    private:
        std::vector<T> &row_ref;

    public:
        explicit ProxyRow(std::vector<T> &row) :
            row_ref(row) {
        }

        auto operator[](int col) -> decltype(row_ref[col]) {
            assert(col >= 0 && size_t(col) < row_ref.size());
            return row_ref[col];
        }
    };

    class ConstProxyRow {
    private:
        const std::vector<T> &row_ref;

    public:
        explicit ConstProxyRow(const std::vector<T> &row) :
            row_ref(row) {
        }

        auto operator[](int col) const -> decltype(row_ref[col]) {
            assert(col >= 0 && size_t(col) < row_ref.size());
            return row_ref[col];
        };
    };

public:
    Grid() :
        data() {
    }
    Grid(int rows, int cols) :
        data(rows, std::vector<T>(cols)) {
    }
    Grid(int rows, int cols, T t) :
        data(rows, std::vector<T>(cols, t)) {
    }

    ProxyRow operator[](int row) {
        assert(row >= 0 && size_t(row) < data.size());
        return ProxyRow(data[row]);
    }

    ConstProxyRow operator[](int row) const {
        assert(row >= 0 && size_t(row) < data.size());
        return ConstProxyRow(data[row]);
    }

    class Coordinate {
    public:
        int row, col;
        Coordinate(int r = 0, int c = 0) :
            row(r), col(c) {
        }

        bool operator==(const Coordinate &other) const {
            return row == other.row && col == other.col;
        }

        bool operator!=(const Coordinate &other) const {
            return !(*this == other);
        }
    };

    class iterator {
    private:
        const Grid *gridPtr;
        Coordinate current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Coordinate;
        using difference_type = std::ptrdiff_t;
        using pointer = Coordinate *;
        using reference = Coordinate &;

        iterator(const Grid *grid, Coordinate coord) :
            gridPtr(grid), current(coord) {
        }

        Coordinate operator*() const {
            return current;
        } // e.g. *it

        iterator &operator++() {
            // 移动到下一个坐标（行优先遍历）
            if (++current.col >= static_cast<int>(gridPtr->numCols())) {
                current.col = 0;
                ++current.row;
            }
            return *this;
        } // e.g. ++it

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        } // e.g. it++

        bool operator==(const iterator &other) const {
            return current == other.current && gridPtr == other.gridPtr;
        } // e.g. it1 == it2

        bool operator!=(const iterator &other) const {
            return !(*this == other);
        } // e.g. it1 != it2
    };

    size_t numRows() const {
        return data.size();
    }
    size_t numCols() const {
        return data.empty() ? 0 : data[0].size();
    }

    bool inBounds(int row, int col) const {
        return row >= 0 && size_t(row) < numRows() && col >= 0 && size_t(col) < numCols();
    }

    friend std::ostream &operator<<(std::ostream &os, const Grid<T> &grid) {
        for (size_t i = 0; i < grid.numRows(); ++i) {
            for (size_t j = 0; j < grid.numCols(); ++j) {
                os << grid[i][j];
                if (j != grid.numCols() - 1) os << " ";
            }
            os << "\n";
        }
        return os;
    }

    // 迭代器开始位置
    iterator begin() const {
        if (numRows() == 0 || numCols() == 0) {
            return end();
        }
        return iterator(this, {0, 0});
    }

    // 迭代器结束位置
    iterator end() const {
        return iterator(this, {static_cast<int>(numRows()), 0});
    }
};

#endif // NEWGRID_H_INCLUDED