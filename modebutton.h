#ifndef MODEBUTTON_H
#define MODEBUTTON_H

#include <QPushButton>

class ModeButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ModeButton(QWidget *parent = nullptr);
    bool isSketchMode() const { return m_sketch; }

signals:
    void modeChanged(bool sketch);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_sketch = true;
    void updateAppearance();   // больше не используем setStyleSheet
};

#endif