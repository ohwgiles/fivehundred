#include "gtest/gtest.h"
#include <QApplication>
#include "card.hpp"
#include <fstream>
#include <lua.hpp>

class LuaFixture : public ::testing::Test {
public:
	LuaFixture() {
		L = lua_open();
	    luaL_openlibs(L);
	}
	void SetUp() {
		ASSERT_TRUE(luafile != NULL);
		ASSERT_EQ(luaL_loadfile(L, luafile), 0);
	}
	~LuaFixture() {
		lua_close(L);
	}
	void loadFile() {
	}
	static const char* luafile;
	lua_State* L;
};
const char* LuaFixture::luafile;

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	// we need a QApplication since Cards contain QPixmaps
	QApplication app(argc, argv);
	LuaFixture::luafile = argv[1];
	return RUN_ALL_TESTS();
}

TEST_F(LuaFixture, ValidSyntax) {
	EXPECT_EQ(lua_pcall(L, 0, 0, 0), 0);
}

TEST_F(LuaFixture, HasRequiredFunctions) {
	EXPECT_EQ(lua_pcall(L, 0, 0, 0), 0);
	lua_getglobal(L, "cardFunc");
	EXPECT_TRUE(lua_isfunction(L, -1));
}
