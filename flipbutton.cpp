#include "flipbutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygon>
#include <QDebug>

FlipButton::FlipButton(QWidget *parent)
    : QPushButton(parent)
{
    setFixedSize(70, 40);              // фиксируем размер, чтобы в тулбаре выглядело нормально
    setCursor(Qt::PointingHandCursor);

    m_animation = new QPropertyAnimation(this, "flipProgress", this);
    m_animation->setDuration(400);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
    connect(m_animation, &QPropertyAnimation::finished, this, &FlipButton::finishFlip);
}

void FlipButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_isPlaying && rect().contains(event->pos())) {
        m_isPlaying = true;
        m_animation->setStartValue(0.0);
        m_animation->setEndValue(1.0);
        m_animation->start();
    }
    QPushButton::mouseReleaseEvent(event);
}

void FlipButton::setFlipProgress(qreal value)
{
    m_flipProgress = value;
    update();
}

void FlipButton::finishFlip()
{
    m_isPlaying = false;
    m_statePlay = !m_statePlay;
    m_flipProgress = 0.0;
    update();
    emit toggled(m_statePlay);  // важный сигнал: состояние кнопки после переключения
    emit clicked();             // оставим и стандартный clicked, если нужен
}

void FlipButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- Контур кнопки (тонкая чёрная линия) ---
    int margin = 2;                         // небольшой отступ от краёв
    QRect borderRect = rect().adjusted(margin, margin, -margin, -margin);
    painter.setPen(QPen(Qt::black, 1));     // тонкий чёрный контур
    painter.setBrush(Qt::NoBrush);          // без заливки
    painter.drawRoundedRect(borderRect, 8, 8);

    // --- Анимация переворота (масштаб по вертикали) ---
    qreal scaleY = 1.0 - m_flipProgress;
    if (m_flipProgress > 0.5) {
        scaleY = (m_flipProgress - 0.5) * 2.0;
    }

    int w = width();
    int h = height();
    int currentH = h * scaleY;
    int yOffset = (h - currentH) / 2;
    if (currentH <= 0) return;

    // --- Иконка всегда полностью непрозрачна ---
    bool showPlayIcon;
    if (m_flipProgress < 0.5)
        showPlayIcon = m_statePlay;
    else
        showPlayIcon = !m_statePlay;

    int iconSize = qMin(w, currentH) * 0.6;
    QRect iconRect((w - iconSize) / 2, yOffset + (currentH - iconSize) / 2, iconSize, iconSize);
    painter.setOpacity(1.0);               // теперь всегда видима
    drawIcon(painter, iconRect, showPlayIcon);
}

void FlipButton::drawIcon(QPainter &p, const QRect &rect, bool isPlayIcon)
{
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0x00, 0x7B, 0xFF));  // синий (можно 0x29B6F6)

    if (isPlayIcon) {
        // треугольник (Play)
        QPolygon triangle;
        triangle << QPoint(rect.left(), rect.top())
                 << QPoint(rect.left(), rect.bottom())
                 << QPoint(rect.right(), rect.center().y());
        p.drawPolygon(triangle);
    } else {
        // квадрат (Stop)
        int m = rect.width() * 0.2;
        p.drawRect(rect.adjusted(m, m, -m, -m));
    }
}