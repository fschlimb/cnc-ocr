{% import "common_macros.inc.c" as util -%}
{% import "icnc/macros_icnc.inc" as util -%}

#include "{{g.name}}_context.h"

int {{g.name}}_main()
{
#ifdef _DIST_
    CnC::dist_init< {{g.name}}_context > _dinit;
#endif

    // initialize context parameters
    {%- for line in g.ctxParams %}
    {{line}} /* TODO: Initialize */
    {%- endfor %}

    // create our context
    {{g.name}}_context ctxt( {{util.render_ctxtargs_param(g, types=False, sfx='')}} );
    
    // Put initial items
    {% for output in g.initFunction.outputs %}
    {{util.render_dependence(g, output, instantiate=True)}}
    {%- endfor %}

    // wait for completetion
    ctxt.wait();

    // get the results
    {%- for input in g.finalizeFunction.inputs %}
    {{util.render_dependence(g, input, getput='get', instantiate=True)}}
    {%- endfor %}
    return 0;
}
