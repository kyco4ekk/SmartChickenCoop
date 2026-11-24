
#include <gtest/gtest.h>
#include "bird_counter.h"

TEST(BirdCounterTests, Constructor_InitializesCounts)
{
    BirdCounter counter(20, 15);

    EXPECT_EQ(counter.GetTotal(), 20);
    EXPECT_EQ(counter.GetInside(), 15);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(BirdCounterTests, BirdEntered_IncreasesInsideCount)
{
    BirdCounter counter(20, 15);

    counter.BirdEntered();

    EXPECT_EQ(counter.GetInside(), 16);
    EXPECT_EQ(counter.GetOutside(), 4);
}

TEST(BirdCounterTests, BirdExited_DecreasesInsideCount)
{
    BirdCounter counter(20, 15);

    counter.BirdExited();

    EXPECT_EQ(counter.GetInside(), 14);
    EXPECT_EQ(counter.GetOutside(), 6);
}

TEST(BirdCounterTests, SetInside_UpdatesOutsideCount)
{
    BirdCounter counter(20, 15);

    counter.SetInside(10);

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);
}

TEST(BirdCounterTests, SetOutside_UpdatesInsideCount)
{
    BirdCounter counter(20, 15);

    counter.SetOutside(7);

    EXPECT_EQ(counter.GetInside(), 13);
    EXPECT_EQ(counter.GetOutside(), 7);
}

TEST(BirdCounterTests, BirdEntered_DoesNothingWhenOutsideIsEmpty)
{
    BirdCounter counter(20, 20);

    counter.BirdEntered();

    EXPECT_EQ(counter.GetInside(), 20);
    EXPECT_EQ(counter.GetOutside(), 0);
}

TEST(BirdCounterTests, BirdExited_DoesNothingWhenInsideIsEmpty)
{
    BirdCounter counter(20, 0);

    counter.BirdExited();

    EXPECT_EQ(counter.GetInside(), 0);
    EXPECT_EQ(counter.GetOutside(), 20);
}


TEST(BirdCounterTests, Constructor_RejectsNegativeTotal)
{
    BirdCounter counter(-1, 0);

    EXPECT_FALSE(counter.IsValid());
}

TEST(BirdCounterTests, Constructor_RejectsInvalidInsideCount)
{
    BirdCounter counter(10, 15);

    EXPECT_FALSE(counter.IsValid());
}

TEST(BirdCounterTests, Constructor_RejectsNegativeInsideCount)
{
    BirdCounter counter(10, -1);

    EXPECT_FALSE(counter.IsValid());
}

TEST(BirdCounterTests, InvalidCounter_DoesNotChangeCounts)
{
    BirdCounter counter(10, 15);

    counter.BirdEntered();
    counter.BirdExited();
    counter.SetInside(5);

    EXPECT_EQ(counter.GetTotal(), 0);
    EXPECT_EQ(counter.GetInside(), 0);
    EXPECT_EQ(counter.GetOutside(), 0);
}


TEST(BirdCounterTests, Counts_AlwaysSumUpToTotal)
{
    BirdCounter counter(20, 10);

    EXPECT_EQ(counter.GetInside() + counter.GetOutside(),
              counter.GetTotal());

    counter.BirdEntered();

    EXPECT_EQ(counter.GetInside() + counter.GetOutside(),
              counter.GetTotal());

    counter.BirdExited();

    EXPECT_EQ(counter.GetInside() + counter.GetOutside(),
              counter.GetTotal());

    counter.SetInside(5);

    EXPECT_EQ(counter.GetInside() + counter.GetOutside(),
              counter.GetTotal());

    counter.SetOutside(15);

    EXPECT_EQ(counter.GetInside() + counter.GetOutside(),
              counter.GetTotal());
}


TEST(BirdCounterTests, MultipleMovements_UpdateCountsCorrectly)
{
    BirdCounter counter(20, 10);

    counter.BirdEntered();
    counter.BirdEntered();
    counter.BirdEntered();

    EXPECT_EQ(counter.GetInside(), 13);
    EXPECT_EQ(counter.GetOutside(), 7);

    counter.BirdExited();
    counter.BirdExited();

    EXPECT_EQ(counter.GetInside(), 11);
    EXPECT_EQ(counter.GetOutside(), 9);
}


TEST(BirdCounterTests, SetInside_RejectsInvalidValues)
{
    BirdCounter counter(20, 10);

    counter.SetInside(-1);

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);

    counter.SetInside(21);

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);
}

TEST(BirdCounterTests, SetOutside_RejectsInvalidValues)
{
    BirdCounter counter(20, 10);

    counter.SetOutside(-1);

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);

    counter.SetOutside(21);

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);
}


TEST(BirdCounterTests, ManyMovements_PreserveTotalCount)
{
    BirdCounter counter(100, 50);

    for (int i = 0; i < 1000; i++)
    {
        counter.BirdEntered();
        counter.BirdExited();
    }

    EXPECT_EQ(counter.GetTotal(), 100);
    EXPECT_EQ(counter.GetInside(), 50);
    EXPECT_EQ(counter.GetOutside(), 50);
}


TEST(BirdCounterTests, DoesNotExceedTotalWhenBirdsEnter)
{
    BirdCounter counter(3, 2);

    counter.BirdEntered();
    counter.BirdEntered();
    counter.BirdEntered();

    EXPECT_EQ(counter.GetInside(), 3);
    EXPECT_EQ(counter.GetOutside(), 0);
    EXPECT_EQ(counter.GetTotal(), 3);
}

TEST(BirdCounterTests, DoesNotGoBelowZeroWhenBirdsExit)
{
    BirdCounter counter(3, 1);

    counter.BirdExited();
    counter.BirdExited();
    counter.BirdExited();

    EXPECT_EQ(counter.GetInside(), 0);
    EXPECT_EQ(counter.GetOutside(), 3);
    EXPECT_EQ(counter.GetTotal(), 3);
}

TEST(BirdCounterTests, AllowsZeroBirdsInside)
{
    BirdCounter counter(10, 0);

    EXPECT_TRUE(counter.IsValid());
    EXPECT_EQ(counter.GetInside(), 0);
    EXPECT_EQ(counter.GetOutside(), 10);
}

TEST(BirdCounterTests, AllowsAllBirdsInside)
{
    BirdCounter counter(10, 10);

    EXPECT_TRUE(counter.IsValid());
    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 0);
}

TEST(BirdCounterTests, InvalidSetOperationsPreserveCounts)
{
    BirdCounter counter(10, 5);

    counter.SetInside(-1);

    EXPECT_EQ(counter.GetInside(), 5);
    EXPECT_EQ(counter.GetOutside(), 5);

    counter.SetOutside(11);

    EXPECT_EQ(counter.GetInside(), 5);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(BirdCounterTests, RepeatedMovementsPreserveTotalAndBounds)
{
    BirdCounter counter(10, 5);

    for (int i = 0; i < 100; i++)
    {
        counter.BirdEntered();

        EXPECT_GE(counter.GetInside(), 0);
        EXPECT_GE(counter.GetOutside(), 0);
        EXPECT_EQ(
            counter.GetInside() + counter.GetOutside(),
            counter.GetTotal()
        );

        counter.BirdExited();

        EXPECT_GE(counter.GetInside(), 0);
        EXPECT_GE(counter.GetOutside(), 0);
        EXPECT_EQ(
            counter.GetInside() + counter.GetOutside(),
            counter.GetTotal()
        );
    }
}



