#include <catch.hpp>

SCENARIO("grade school math is not a lie")
{
	GIVEN("the number 1")
	{
		int i = 1;
		
		WHEN("1 is added to it")
		{
			i += 1;
			
			THEN("the answer is 2")
			{
				REQUIRE(i == 2);
			}
		}
	}
}
