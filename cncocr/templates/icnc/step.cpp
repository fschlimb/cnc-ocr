{% import "common_macros.inc.c" as util with context -%}
{% import "icnc/macros_icnc.inc" as util with context -%}
{% set stepfun = g.stepFunctions[targetStep] -%}

#include "{{g.name}}_context.h"

int {{stepfun.collName}}_step::execute( const {{stepfun.collName}}_tag_t & tag, {{g.name}}_context & ctxt ) const
{
    {{g.name}}_context * ctx = &ctxt;
    {% if stepfun.tag|count > 1 -%}
    {%- for arg in stepfun.tag -%}
    const cnc_tag_t {{arg}} = tag.{{arg}};
    {%- endfor -%}
    {%- elif stepfun.tag|count > 0 -%}
    const cnc_tag_t {{stepfun.tag[0]}} = tag;
    {% endif %}

    {{util.render_ctxtargs_init(g, pfx='ctxt.', types=True)}}

    // Get input items
    {%- for input in stepfun.inputs %}
    {{util.render_dependence(g, "ctxt.", input, getput='get', instantiate=True, todo=False)}}
    {%- endfor %}

    // TODO: here goes your computation
    // creating output handles and assign values
    {%- for output in stepfun.outputs %}
    {{util.render_dependence(g, "ctxt.", output, getput=False, instantiate=True)}}
    {%- endfor %}

    // TODO: create output
    {% for output in stepfun.outputs -%}
    {{util.render_dependence(g, "ctxt.", output)}}
    {%- endfor %}
    return CnC::CNC_Success;
}
