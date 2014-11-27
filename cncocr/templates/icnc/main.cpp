{% import "common_macros.inc.c" as util -%}
{% import "icnc/macros_icnc.inc" as util -%}

#include "{{g.name}}_context.h"
#include "{{g.name}}_defs.h"

int {{g.name}}_main( {{g.name ~ "Args * args" ~ (", " if g.initFunction.tag or g.ctxParams|count else " ")}}
{%- if g.initFunction.tag -%}
{%- for a in g.initFunction.tag -%}
cnc_tag_t {{a}}{{", " if not loop.last or g.ctxParams|count}}
{%- endfor -%}
{%- endif -%}
{{util.render_ctxtargs_param(g, pfx='')}} )
{
#ifdef _DIST_
    CnC::dist_cnc_init< {{g.name}}_context > _dinit;
#endif

    // create our context
    {{g.name}}_context ctxt{{("( " ~ util.render_ctxtargs_param(g, types=False, pfx='') ~ " )") if g.ctxParams|count}};
    
    // Put initial items
    {% for output in g.initFunction.outputs %}
    {{util.render_dependence(g, "ctxt.", output, instantiate=True)}}
    {%- endfor %}

    // wait for completetion
    ctxt.wait();

    {
        // get the results
        {%- for input in g.finalizeFunction.inputs %}
        {{util.render_dependence(g, "ctxt.", input, getput='get', instantiate=True)}}
        {%- endfor %}
    }
    return 0;
}

int main( const int argc, const char*[] ) 
{
    /* TODO: parse arguments etc. */

    {% if g.initFunction.tag %}
    // TODO: Initialize these tag variables using args
    cnc_tag_t {{ g.initFunction.tag|join(", ") }};
    {% endif %}
    {% if g.ctxParams|count > 0 %}
    // initialize context parameters
    {% for line in g.ctxParams %}
    {{line}} /* TODO: Initialize */
    {%- endfor %}
    {% endif %}
    
    {{g.name}}Args args /* = TODO: Initialize args struct */;

    return {{g.name}}_main({{" &args" ~ (", " if g.initFunction.tag or g.ctxParams|count else " ")}}
    {%- if g.initFunction.tag -%}
    {%- for a in g.initFunction.tag -%}
    {{ " " ~ a ~ (", " if not loop.last or g.ctxParams|count > 0)}}
    {%- endfor -%}
    {%- endif -%}
    {{util.render_ctxtargs_param(g, types=False, pfx='')}} );
}
