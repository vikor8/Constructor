#ifndef FLIPBUTTON_H
#define FLIPBUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>

class FlipButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(qreal flipProgress READ flipProgress WRITE setFlipProgress)

public:
    explicit FlipButton(QWidget *parent = nullptr);
    bool isPlaying() const { return m_isPlaying; }
    qreal flipProgress() const { return m_flipProgress; }
    QSize sizeHint() const override { return QSize(70, 40); } // подходящий размер для тулбара

signals:
    void toggled(bool playState); // испускается после завершения анимации

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void setFlipProgress(qreal value);
    void finishFlip();

private:
    QPropertyAnimation *m_animation;
    qreal m_flipProgress = 0.0;
    bool m_isPlaying = false;
    bool m_statePlay = true; // true = показываем "Пуск", false = "Стоп"
    void drawIcon(QPainter &p, const QRect &rect, bool isPlayIcon);
};

#endif // FLIPBUTTON_H