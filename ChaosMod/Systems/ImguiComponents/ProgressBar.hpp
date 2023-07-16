#pragma once

class ProgressBar final
{
        float progress = 0.0f;

    public:
        void SetProgressPercentage(float percent);
        void Render() const;
};
