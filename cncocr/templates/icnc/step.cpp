{% import "common_macros.inc.c" as util -%}
{% import "icnc/macros_icnc.inc" as util -%}
{% set stepfun = g.stepFunctions[targetStep] -%}

#include "{{g.name}}_context.h"

int {{stepfun.collName}}_step::execute( const {{stepfun.collName}}_tag_t & tag, {{g.name}}_context & ctxt ) const
{
    // Get input items
    {%- for input in stepfun.inputs %}
    {{util.render_dependence(g, input, getput='get', instantiate=True, todo=False)}}
    {%- endfor %}

    // TODO: here goes your computation
    // creating output handles and assign values
    {%- for output in stepfun.outputs %}
    {{util.render_dependence(g, output, getput=False, instantiate=True)}}
    {%- endfor %}

    // TODO: create output
    {% for output in stepfun.outputs -%}
    {{util.render_dependence(g, output)}}
    {%- endfor %}
    return CnC::CNC_Success;
}
