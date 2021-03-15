/*
 * BaseNode.hpp
 * Homie Node for generic nodes.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

class BaseNode : public HomieNode
{
protected:
    const char *cIndent = "  ◦ ";
    char *_caption{};

    void fixRange(float *value, float min, float max);
    virtual void printCaption();

public:
    explicit BaseNode(const char *id, const char *name, const char *type);
};