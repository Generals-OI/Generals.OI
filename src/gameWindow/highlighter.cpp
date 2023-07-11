#include "gameWindow.h"

Highlighter::Highlighter(QTextDocument *parent, int &cntPlayer, std::vector<PlayerInfo> &playersInfo)
    : QSyntaxHighlighter(parent) {

    for (int i = 1; i <= cntPlayer; i++) {
        HighlightingRule rule;
        QString expr = transExpr(playersInfo[i].nickName);
        qDebug() << "[highlighter.cpp]" << expr;
        rule.expr.setPattern(expr);
        rule.format.setForeground(QColor(strColor[i]));
        rule.format.setFontWeight(QFont::Normal);
        rules.append(rule);
    }
}

void Highlighter::highlightBlock(const QString &text) {
    for (const auto &rule: qAsConst(rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.expr.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
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
    return QString("(%1)").arg(res);
}
