#include <vector>
#include "Boid.h"
#include "boidHandler.h"

class gridHandler : public boidHandler {
private:
    struct cell {
        Vector2 topLeft;
        Vector2 bottomRight;

        std::vector<Boid*> boidsInCell;
    };

    // grid parameters
    int width;
    int height;

    std::vector<std::vector<std::vector<cell>>> grid;

    const int maxBoidsPerCell = 10;

public:
    gridHandler(int w, int h, int cSize) : width(w), height(h) {
        width = w;
        height = h;

        grid.resize(1);
        grid[0].resize(1);
        grid[0][0].push_back(cell{ {0,0}, {(float)w, (float)h}, {} });
    }

private:
    void clearGrid() {
        for (auto& row : grid) {
            for (auto& col : row) {
                col.clear();
            }
        }
    }

    void splitBoidsIntoGrid(std::vector<Boid*>& boids) {
        // Reinitialize grid with a single cell covering the whole area
        grid.clear();
        grid.resize(1);
        grid[0].resize(1);
        grid[0][0].clear();
        grid[0][0].push_back(cell{ {0,0}, {(float)width, (float)height}, {} });

        // Add all boids to the initial cell
        for (auto& b : boids) {
            grid[0][0][0].boidsInCell.push_back(b);
        }

        // subdivide cells until all cells have boids <= maxBoidsPerCell
        subdivideCells();
    }

    void subdivideCells() {
        if (allCellsSplit()) return;

        // loop through all cells and split those that have too many boids
        std::vector<std::vector<std::vector<cell>>> newGrid;
        
        // go through existing grid
        for (size_t i = 0; i < grid.size(); i++) {
            // rows
            newGrid.push_back({});
            for (size_t j = 0; j < grid[i].size(); j++) {
                // cols
                newGrid[i].push_back({});
                for (const auto& c : grid[i][j]) {
                    if (c.boidsInCell.size() > (size_t)maxBoidsPerCell) {
                        // split cell into 4
                        float midX = (c.topLeft.x + c.bottomRight.x) / 2.0f;
                        float midY = (c.topLeft.y + c.bottomRight.y) / 2.0f;
                        cell c1{ c.topLeft, {midX, midY}, {} };
                        cell c2{ {midX, c.topLeft.y}, {c.bottomRight.x, midY}, {} };
                        cell c3{ {c.topLeft.x, midY}, {midX, c.bottomRight.y}, {} };
                        cell c4{ {midX, midY}, c.bottomRight, {} };

                        // redistribute boids into new cells
                        for (auto bPtr : c.boidsInCell) {
                            Vec2 p = bPtr->pos;
                            if (p.x <= midX && p.y <= midY) {
                                c1.boidsInCell.push_back(bPtr);
                            } else if (p.x > midX && p.y <= midY) {
                                c2.boidsInCell.push_back(bPtr);
                            } else if (p.x <= midX && p.y > midY) {
                                c3.boidsInCell.push_back(bPtr);
                            } else {
                                c4.boidsInCell.push_back(bPtr);
                            }
                        }
                        newGrid[i][j].push_back(c1);
                        newGrid[i][j].push_back(c2);
                        newGrid[i][j].push_back(c3);
                        newGrid[i][j].push_back(c4);
                    } else {
                        // keep cell as is
                        newGrid[i][j].push_back(c);
                    }
                }
            }
        }
        grid = newGrid;
        
        // Recursively subdivide until all cells are within limit
        subdivideCells();
    }

    bool allCellsSplit() {
        for (const auto& row : grid) {
            for (const auto& col : row) {
                for (const auto& c : col) {
                    if (c.boidsInCell.size() > (size_t)maxBoidsPerCell) return false;
                }
            }
        }
        return true;
    }

public:

    // Get boids within a certain range of a position
    // checks boids only in relevant grid cells
    // relevant grid cells are any that are partially or fully within range of position
    std::vector<Boid*> getBoidsInRange(const Vec2& position, float range) override {
        std::vector<Boid*> nearbyBoids;
        float range2 = range * range;

        std::vector<const cell*> relevantCells;

        // find all cells that intersect with the range circle
        for (const auto& row : grid) {
            for (const auto& col : row) {
                for (const auto& c : col) {
                    // check if cell intersects with circle
                    float closestX = std::max(c.topLeft.x, std::min(position.x, c.bottomRight.x));
                    float closestY = std::max(c.topLeft.y, std::min(position.y, c.bottomRight.y));
                    float distX = position.x - closestX;
                    float distY = position.y - closestY;
                    float distSquared = distX * distX + distY * distY;
                    if (distSquared <= range2) {
                        relevantCells.push_back(&c);
                    }
                }
            }
        }

        // gather boids from relevant cells
        for (const auto& cPtr : relevantCells) {
            for (const auto& bPtr : cPtr->boidsInCell) {
                // double check within range
                float d2 = distSquared(position, bPtr->pos);
                if (d2 <= range2) {
                    nearbyBoids.push_back(bPtr);
                }
            }
        }

        return nearbyBoids;
    }

    void setBoids(const std::vector<Boid*>& allBoids) override {
        splitBoidsIntoGrid(const_cast<std::vector<Boid*>&>(allBoids));
    }

    void drawGrid() {
        for (const auto& row : grid) {
            for (const auto& col : row) {
                for (const auto& c : col) {
                    DrawRectangleLines((int)c.topLeft.x, (int)c.topLeft.y,
                                       (int)(c.bottomRight.x - c.topLeft.x),
                                       (int)(c.bottomRight.y - c.topLeft.y),
                                       GREEN);
                }
            }
        }
    }

};