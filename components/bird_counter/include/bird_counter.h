
#pragma once

class BirdCounter
{
public:
    BirdCounter(int total, int inside);

    bool IsValid() const;

    int GetTotal() const;
    int GetInside() const;
    int GetOutside() const;

    void BirdEntered();
    void BirdExited();

    void SetInside(int count);
    void SetOutside(int count);

private:
    int total;
    int inside;
    int outside;
    bool valid;
};
