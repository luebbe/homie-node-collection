/*
 * BaseNode.hpp
 * Homie Node for generic nodes.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "BaseNode.hpp"

BaseNode::BaseNode(const char *id, const char *name, const char *type)
    : HomieNode(id, name, type),
      _caption(0)
{
}

void BaseNode::fixRange(float *value, float min, float max)
{
    if (isnan(*value))
    {
        return;
    }
    else if (*value < min)
    {
        *value = min;
    }
    else if (*value > max)
    {
        *value = max;
    };
}

void BaseNode::printCaption()
{
    Homie.getLogger() << _caption << endl;
}
