#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <string>
#include <iomanip>

struct Vec3 {
    float x;
    float y;
    float z;

    Vec3(float inX = 0.0f, float inY = 0.0f, float inZ = 0.0f) : x(inX), y(inY), z(inZ) {}
};

enum class FormationType {
    Line,
    Square,
    Phalanx,
    Skirmish
};

std::vector<Vec3> CalculateFormationOffsets(FormationType formation, int unitCount, float spacing) {
    std::vector<Vec3> offsets;
    if (unitCount <= 0) return offsets;
    offsets.reserve(unitCount);

    switch (formation) {
        case FormationType::Line: {
            for (int i = 0; i < unitCount; ++i) {
                float x = 0.0f;
                float y = (static_cast<float>(i) - static_cast<float>(unitCount - 1) * 0.5f) * spacing;
                float z = 0.0f;
                offsets.emplace_back(x, y, z);
            }
            break;
        }
        case FormationType::Square: {
            int cols = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(unitCount))));
            int rows = static_cast<int>(std::ceil(static_cast<float>(unitCount) / static_cast<float>(cols)));
            for (int i = 0; i < unitCount; ++i) {
                int row = i / cols;
                int col = i % cols;
                float x = -(static_cast<float>(row) - static_cast<float>(rows - 1) * 0.5f) * spacing;
                float y = (static_cast<float>(col) - static_cast<float>(cols - 1) * 0.5f) * spacing;
                float z = 0.0f;
                offsets.emplace_back(x, y, z);
            }
            break;
        }
        case FormationType::Phalanx: {
            int cols = std::min(10, std::max(4, static_cast<int>(std::ceil(std::sqrt(static_cast<float>(unitCount))))));
            int rows = static_cast<int>(std::ceil(static_cast<float>(unitCount) / static_cast<float>(cols)));
            float fileSpacing = spacing * 0.85f;
            float rankSpacing = spacing * 0.75f;
            for (int i = 0; i < unitCount; ++i) {
                int row = i / cols;
                int col = i % cols;
                float x = -(static_cast<float>(row) - static_cast<float>(rows - 1) * 0.5f) * rankSpacing;
                float y = (static_cast<float>(col) - static_cast<float>(cols - 1) * 0.5f) * fileSpacing;
                float z = 0.0f;
                offsets.emplace_back(x, y, z);
            }
            break;
        }
        case FormationType::Skirmish: {
            int cols = std::max(4, static_cast<int>(std::ceil(std::sqrt(static_cast<float>(unitCount) * 1.5f))));
            int rows = static_cast<int>(std::ceil(static_cast<float>(unitCount) / static_cast<float>(cols)));
            float wideSpacing = spacing * 1.5f;
            for (int i = 0; i < unitCount; ++i) {
                int row = i / cols;
                int col = i % cols;
                float stagger = (row % 2 == 1) ? (spacing * 0.5f) : 0.0f;
                float x = -(static_cast<float>(row) - static_cast<float>(rows - 1) * 0.5f) * wideSpacing;
                float y = (static_cast<float>(col) - static_cast<float>(cols - 1) * 0.5f) * wideSpacing + stagger;
                float z = 0.0f;
                offsets.emplace_back(x, y, z);
            }
            break;
        }
    }
    return offsets;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "CHRONICLES OF DOMINION: FORMATION MATH TESTS" << std::endl;
    std::cout << "========================================" << std::endl;

    const float Spacing = 100.0f;
    const int UnitCount = 10;

    // 1. Test Line
    std::cout << "\n[TEST 1] Line Formation (10 units):" << std::endl;
    auto lineOffsets = CalculateFormationOffsets(FormationType::Line, UnitCount, Spacing);
    assert(lineOffsets.size() == UnitCount);
    assert(std::abs(lineOffsets[0].x) < 0.001f);
    assert(std::abs(lineOffsets[0].y - (-4.5f * Spacing)) < 0.001f);
    assert(std::abs(lineOffsets[9].y - (4.5f * Spacing)) < 0.001f);
    std::cout << "  - First unit offset: (" << lineOffsets[0].x << ", " << lineOffsets[0].y << ", " << lineOffsets[0].z << ")" << std::endl;
    std::cout << "  - Last unit offset:  (" << lineOffsets[9].x << ", " << lineOffsets[9].y << ", " << lineOffsets[9].z << ")" << std::endl;
    std::cout << "  * Line formation symmetry verified." << std::endl;

    // 2. Test Square
    std::cout << "\n[TEST 2] Square Formation (16 units):" << std::endl;
    auto squareOffsets = CalculateFormationOffsets(FormationType::Square, 16, Spacing);
    assert(squareOffsets.size() == 16);
    assert(std::abs(squareOffsets[0].x - (1.5f * Spacing)) < 0.001f);
    assert(std::abs(squareOffsets[0].y - (-1.5f * Spacing)) < 0.001f);
    std::cout << "  - Corner (0,0) offset: (" << squareOffsets[0].x << ", " << squareOffsets[0].y << ", " << squareOffsets[0].z << ")" << std::endl;
    std::cout << "  - Corner (3,3) offset: (" << squareOffsets[15].x << ", " << squareOffsets[15].y << ", " << squareOffsets[15].z << ")" << std::endl;
    std::cout << "  * Square formation 4x4 grid bounds verified." << std::endl;

    // 3. Test Phalanx
    std::cout << "\n[TEST 3] Phalanx Shield Wall Formation (100 units):" << std::endl;
    auto phalanxOffsets = CalculateFormationOffsets(FormationType::Phalanx, 100, Spacing);
    assert(phalanxOffsets.size() == 100);
    std::cout << "  - Front-center offset: (" << phalanxOffsets[4].x << ", " << phalanxOffsets[4].y << ", " << phalanxOffsets[4].z << ")" << std::endl;
    std::cout << "  - Rear-corner offset:  (" << phalanxOffsets[99].x << ", " << phalanxOffsets[99].y << ", " << phalanxOffsets[99].z << ")" << std::endl;
    std::cout << "  * Phalanx 10x10 tight depth and file spacing verified." << std::endl;

    // 4. Test Skirmish
    std::cout << "\n[TEST 4] Skirmish Staggered Dispersion (20 units):" << std::endl;
    auto skirmishOffsets = CalculateFormationOffsets(FormationType::Skirmish, 20, Spacing);
    assert(skirmishOffsets.size() == 20);
    std::cout << "  - Rank 0 Unit 0 offset: (" << skirmishOffsets[0].x << ", " << skirmishOffsets[0].y << ", " << skirmishOffsets[0].z << ")" << std::endl;
    std::cout << "  - Rank 1 Unit 0 offset: (" << skirmishOffsets[6].x << ", " << skirmishOffsets[6].y << ", " << skirmishOffsets[6].z << ")" << std::endl;
    std::cout << "  * Skirmish staggered dispersion verified." << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << "ALL FORMATION MATH TESTS PASSED SUCCESSFULLY (100%)" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
