#include "modebutton.h"
#include <QPainter>
#include <QMouseEvent>

ModeButton::ModeButton(QWidget *parent) : QPushButton(parent), m_sketch(true)
{
    setFixedSize(70, 40);            // такой же размер, как у FlipButton
    setCursor(Qt::PointingHandCursor);
}

void ModeButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int margin = 4;
    // Только внутренний прямоугольник
    QRect innerRect(margin, margin, w - 2*margin, h - 2*margin);
    painter.setPen(Qt::NoPen);

    // Цвет зависит от режима
    if (m_sketch) {
        painter.setBrush(QColor(0xFF, 0xA5, 0x00));  // оранжевый
    } else {
        painter.setBrush(QColor(0x4C, 0xAF, 0x50));  // зелёный
    }
    painter.drawRoundedRect(innerRect, 8, 8);

    // Текст
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(14);
    painter.setFont(font);
    QString text = m_sketch ? "Эск" : "Чер";
    painter.drawText(innerRect, Qt::AlignCenter, text);
}

void ModeButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        m_sketch = !m_sketch;
        update();                     // перерисовка
        emit modeChanged(m_sketch);   // оповещаем
    }
    QPushButton::mouseReleaseEvent(event);
}

void ModeButton::updateAppearance()
{
    // теперь не нужно
}