#include "gameWindow.h"

extern QString strFontBold;

Highlighter::Highlighter(QTextDocument *parent, int &cntPlayer, std::vector<PlayerInfo> &playersInfo, const QFont &font)
        : QSyntaxHighlighter(parent) {
    QFont boldFont(strFontBold, font.pointSize());
    addRule("Generals.OI", QColor(strColor[0]), boldFont);
    addRule("Server", QColor(strColor[0]), boldFont);
    for (int i = 1; i <= cntPlayer; i++)
        addRule(playersInfo[i].nickName, QColor(strColor[i]), font);
}

void Highlighter::addRule(const QString &str, QColor color, const QFont &font) {
    HighlightingRule rule;
    QString expr = transExpr(str);
    qDebug() << "[highlighter.cpp]" << expr;
    rule.expr.setPattern(expr);
    rule.format.setForeground(color);
    rule.format.setFont(font);
    rules.append(rule);
}

void Highlighter::highlightBlock(const QString &text) {
    for (const auto &rule: qAsConst(rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.expr.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat((int) match.capturedStart(), (int) match.capturedLength(), rule.format);
        }
    }
}

QString Highlighter::transExpr(const QString &str) {
    const QString specials = "$()*+.[]?\\^{}|";
    QString res{};
    for (auto c: str) {
        if (specials.indexOf(c) != -1)
            res.append("\\");
        res.append(c);
    }
    return QString("((@%1\\s)|(%2:))").arg(res, res);
}
