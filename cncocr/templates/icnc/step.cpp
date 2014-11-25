{% import "common_macros.inc.c" as util -%}
{% import "icnc/macros_icnc.inc" as util -%}
{% set stepfun = g.stepFunctions[targetStep] -%}

#include "{{g.name}}_context.h"

int {{stepfun.collName}}_step::execute( const {{stepfun.collName}}_tag_t & tag, {{g.name}}_context & ctxt ) const
{
    // Get input items
    {%- for input in stepfun.inputs %}
    {{g.itemDeclarations[input.collName].type}} {{input.binding}};
    {{input.collName}}.get( {{input.collName}}_key_t(
    {%- for k in input.key -%}
    {{k.expr ~ (", " if not loop.last)}}
    {%- endfor -%}
    ), {{input.binding}} );
    {%- endfor %}

    // TODO: here goes your computation
    // creating output handles and assign values

    {%- for output in stepfun.outputs %}
    {% if output.kind == 'ITEM' -%}
    {{g.itemDeclarations[output.collName].type}} {{output.binding}}_handle = /* TODO: computation */;
    {% endif %}
    {%- endfor %}

    // TODO: create output
    {%- for output in stepfun.outputs -%}
    {{util.render_put(output)}}
    {% endfor %}
    return CnC::CNC_Success;
}
